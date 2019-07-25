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

	if(fsm->StateFirstEntry){
		qTraceMessage("[STATE] : State_Init\r\n");
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2 , GPIO_PIN_SET);
		 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2 , GPIO_PIN_RESET);
		 HAL_ADC_Start(&hadc1);
		 HAL_ADC_PollForConversion(&hadc1, 1000);
		 qDebugDecimal(HAL_ADC_GetValue(&hadc1));
		 HAL_ADC_Stop(&hadc1);

		//qRBufferPush(&SigFox_UplinkQueue, &DataFrame);
		qRBufferPush(&SigFox_UplinkQueue, &DataFrame);
	}


	if(qRBufferEmpty(&SigFox_UplinkQueue)){
		fsm->NextState = State_Sleep;
	}
	return qSM_EXIT_SUCCESS;
}


