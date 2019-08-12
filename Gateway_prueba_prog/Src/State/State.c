/*
 * State.c
 *
 *  Created on: 24/04/2019
 *      Author: julian
 */
/*3 EVENTOS DE BOTON*/
/***************   NO LEER-NO LEER- NO LEER*******************************************/

/** PENDIENTES 24-04-2019
 *1- Funcion para tiempos keep alive en d�as, esta con multiplos de d�as, despierta verifica y duerme , hasta el keep alive ah� transmite
 * 2-Contar los fallos y dada x cantidad de fallos por respuestas de comandos reiniciar el modulo
 * 4- Secuencia de colores y de Leds (RGB-PWM??)
 * */

#include "../Inc/State/State.h"


/***********************************************************************************************/
void State_Failure(qSM_t *fsm){
}

/***********************************************************************************************/
qSM_Status_t State_Sleep(qSM_t *fsm){
	WSSFM1XRX_Return_t RetValue;
	int Ret;
	if(fsm->StateFirstEntry){
		RetValue = WSSFM1XRX_WakeUP(&SigfoxModule,WSSFM1XRX_Wait_Block);
		qTraceMessage("[STATE] : State_Sleep\r\n");

	}

	if(SigFoxData.WilsolService_Status  == WSSFM1XRX_STATUS_MODULE_NOT_INITIALIZED || SigFoxData.WilsolService_Status  == WSSFM1XRX_STATUS_MODULE_INITIALIZED || SigFoxData.WilsolService_Status  == WSSFM1XRX_STATUS_SENT_MESSAGE)
	{
		if(WSSFM1XRX_OK_RESPONSE ==  WSSFM1XRX_Sleep(&SigfoxModule,WAIT) ){
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2 , GPIO_PIN_SET);
			PrintString(&hlpuart1,(uint8_t *)"-----------------------Core Sleep-------------------\r\n");
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2 , GPIO_PIN_SET);
			 HAL_GPIO_WritePin(GPIOC, Gpio4_Bus1_Shutdown_Pin, GPIO_PIN_RESET);
			__HAL_RCC_PWR_CLK_ENABLE();
			HAL_PWR_EnterSTANDBYMode();
		}
	}
	return qSM_EXIT_SUCCESS;
}
/***********************************************************************************************/
qSM_Status_t State_Init(qSM_t *fsm){
	float ADC_0,ADC_1,ADC_2;

	if(fsm->StateFirstEntry){
		qTraceMessage("[STATE] : State_Init\r\n");
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2 , GPIO_PIN_SET);
		 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2 , GPIO_PIN_RESET);


		 ADC_0 = App_ADCReadSingleChanne(&hadc1, 4)*(3.3/4095.0)/0.452;
		 ADC_1 = App_ADCReadSingleChanne(&hadc1, 9)*(3.3/4095.0)/0.599;
		 ADC_2 = App_ADCReadSingleChanne(&hadc1, 3)*(3.3/4095.0);
		 //AN_IN1= ADC123_IN3(CORRIENTE)  AN_IN2 = ADC123_IN4(0-10) AN_IN3 = ADC12_IN9(0-5)
		 DataFrame.ADC_0 = ADC_0*58.5 + 2340; //ADC123_IN4(0-10)
		 DataFrame.ADC_1 = ADC_1*58.5 + 2340; //ADC12_IN9(0-5)
		 DataFrame.ADC_2 = ADC_2*58.5 + 2340;

		 qDebugFloat(ADC_0);
		 qDebugFloat(ADC_1);
		 qDebugFloat(ADC_2);
		 //DataFrame.Others[6] = 0x06;
		 DataFrame.DI1=1;
		 DataFrame.DI2=1;

		 //DataFrame.periodic = 1;
		//qRBufferPush(&SigFox_UplinkQueue, &DataFrame);
		qRBufferPush(&SigFox_UplinkQueue, &DataFrame);
	}


	if(qRBufferEmpty(&SigFox_UplinkQueue)){// && LoRaReadyToUplink){
		//qDebugMessage("mac tx cnf 1 000000000006000000A2B9c7\r\n");  // 06 = di2 =1 di1 = 1 A2B 2603 = 4.5v  de 5    9c7 de 0-5v
		//qDebugMessage("AT$SF=000000000006000000A2B9c7\r\n");
		LoRaReadyToUplink = 0;
		qTraceMem(&DataFrame,sizeof(DataFrame));
		fsm->NextState = State_Sleep;

	}
	return qSM_EXIT_SUCCESS;
}


