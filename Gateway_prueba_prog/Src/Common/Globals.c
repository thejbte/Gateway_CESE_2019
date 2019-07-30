/*
 * Globals.c
 *
 *  Created on: 30/01/2019
 *      Author: julian
 */

#include <Common/Globals.h>

volatile qRespHandler_t ResponseObject;
volatile qRespHandler_t ResponseObjectLora;

/** Antirebore */
DebounceData_t DebounceData;
volatile Debounce_Flag_t Debounce_Flag;

/**/
uint8_t ContTime = 0;

/*Struct Led rgb*/
ColorRGB_t Color;
/** Scheduler */

qTask_t Task_ApplicationFSM, Task_UplinkDispatcher, Task_Wisol_Service, Task_Leds,Task_LoRaWANService;

qIOEdgeCheck_t InputCheck;
qIONode_t InputButton;
qSTimer_t Timeout = QSTIMER_INITIALIZER;
qRBuffer_t SigFox_UplinkQueue;
uint8_t mQueue_Stack[3*sizeof(SigfoxServiceRequest_t)]={0};

/** otros */
volatile uint8_t FlagCont=0;
uint8_t ReportTimeHour = 0;
uint8_t ReportTimeMinute = 0;
uint8_t ReportTimeSecond = 0;

uint8_t DownLink =0;

UART_HandleTypeDef hlpuart1;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/** Buffer Rx Frame*/
char BufferRxFrame[WSSFM1XRX_BUFF_RX_FRAME_LENGTH];

/** Buffer Tx Frame*/
char BufferTxFrame[WSSFM1XRX_BUFF_TX_FRAME_LENGTH];


FLASH_EraseInitTypeDef FLASHMEM;
/******************Sigfox library*******************************/
WSSFM1XRXConfig_t SigfoxModule;


/*Wisol service*/
SigFox_Info_t SigFoxData;

uint8_t LoRaReadyToUplink;
/*Others*/
uint8_t FlagChangeFrequencyPulsations = 0;


UART_BufferData_t UART_RX;
UART_BufferData_t UART_RX3;


void RSTCtrl_Sigfox(uint8_t sValue){
	if(sValue) HAL_GPIO_WritePin(GPIOB, Gpio2_Bus1_Wakeup_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(GPIOB, Gpio2_Bus1_Wakeup_Pin, GPIO_PIN_RESET);
}
void RST2Ctrl_Sigfox(uint8_t sValue){
	if(sValue) HAL_GPIO_WritePin(GPIOB, Gpio3_Bus1_Rst_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(GPIOB, Gpio3_Bus1_Rst_Pin, GPIO_PIN_RESET);
}


/*****Funci�n TX para envolver(Wrap) con Libreria sigfox********/
void UART_SIGFOX_TX_STM(void * Sp, char c){
	HAL_UART_Transmit(&huart2,(uint8_t*)&c,USART_TX_AMOUNT_BYTES,USART_TIMEOUT);
}
/* BUS2 UART3*/
void PutCharWrapperUart_3(void *sp, const char c){
	while((__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) ? SET : RESET) == RESET) {}
	huart3.Instance->TDR = (c & (uint8_t)0xFFU);
}

void PutStringWrapperUart_3(void *sp, const char *s){
	while(*s){
		PutCharWrapperUart_3(NULL,*s++);
	}
}



/*Transmitir uart 1 no hal*/ /*Bus 1 */
void PutCharWrapperUart_1(void *sp, const char c){
	while((__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) ? SET : RESET) == RESET) {}
	huart2.Instance->TDR = (c & (uint8_t)0xFFU);
}

void PutCharWrapperUart_2(void *sp, const char c){
	while((__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_TC) ? SET : RESET) == RESET) {}
	hlpuart1.Instance->TDR = (c & (uint8_t)0xFFU);
}


void PutStringWrapperUart_2(void *sp, const char *s){
	while(*s){
		PutCharWrapperUart_2(NULL,*s++);
	}
}

/************************Data Sigfox *************************************/
DataFrame_t DataFrame;

/*flags globales*/
volatile Flags_t Flags_globals;

/*Tipo de datos*/
tipo_t xtypes;

/*Enviar por tx Debug var ************************************************/
void PrintStringVar(UART_HandleTypeDef *huart,uint8_t *Str, uint16_t  Variable){
	uint8_t length;
	length = strlen((const char *)(Str) );
	uint8_t Buf[length];
	memset((void *)Buf,0,sizeof(Buf));
	sprintf((char *)Buf,(const char *)(Str),Variable);
	PutStringWrapperUart_2(NULL, Buf);
	//HAL_UART_Transmit(huart,(uint8_t *)Buf,sizeof(Buf),USART_TIMEOUT);
}
/** Enviar por tx Debug*****************************************************/
void PrintString(UART_HandleTypeDef *huart,uint8_t *pData){
	//uint8_t length;
	//length = strlen((const char *)(pData) );
	PutStringWrapperUart_2(NULL, pData);
	//HAL_UART_Transmit(huart,pData,length,USART_TIMEOUT);
}


/** Funcion que decodifica la trama entrante *******************************/
WSSFM1XRX_DL_Return_t DiscrimateFrameType(WSSFM1XRXConfig_t *obj){
	uint16_t tempReg;

	/* Discriminate the frame type */
	switch(obj->DL_NumericFrame[WSSFM1XRX_DL_CTRLREG] >> 4){ /* 4 most significant bits */

	case WSSFM1XRX_DL_FRAME_REPORT_TIME_AND_TURN_OFF_MOTO:
		tempReg = (obj->DL_NumericFrame[WSSFM1XRX_DL_TREP] << 8) | obj->DL_NumericFrame[WSSFM1XRX_DL_TREP + 1]; 	/* junto los 2 bytes en 1*/
		//Flags_globals.DL_Power_ON = numericFrame[DL_P_ON_OFF] <= 0 ? 0 :1;
		if(tempReg >= WSSFM1XRX_DL_MIN_REPORT_TIME){

			if(tempReg != obj->UL_ReportTimeS){ /*Si el dato es diferente*/
				obj->UL_ReportTimeS = tempReg;

			}
			return WSSFM1XRX_DL_TIME_OK;
			/* TODO: Add callback here */
		}

		break;

	default:
		break;
	}/* End switch */

	return WSSFM1XRX_DL_SUCCESS;
}

/*Blink led para diferencias Pulsaciones ============================================================================*/
void IndicatorBlink(float t_ms,char  color){
	static qSTimer_t TimeoutLed = QSTIMER_INITIALIZER;
	static int Ret=1;
	if(t_ms != 0){
		Ret = LedColor(color,Ret );
		if(qSTimerFreeRun(&TimeoutLed,t_ms)){
			Ret = LedColor(color,!Ret );
		}
	}else Ret = LedColor(color,1);

}

GPIO_PinState LedColor(char color, GPIO_PinState state){}
/*======================================================================================================================*/
/*Obtener las fuentes de interrupci�n*/
void get_system_reset_cause(void){
	__HAL_RCC_PWR_CLK_ENABLE(); /*para detectar sorce*/
	//	/** source event wakeup*/

	/*Wup Pin PWR_CSR_WUF*/


//	if(PWR-> CSR  & PWR_FLAG_WU ){
//		qDebugMessage("Wake up Transmision Pin\r\n");
//		PWR-> CR |=  PWR_CR_CWUF;
//		DataFrame.periodic = 1;
//	}else
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)){
		/*Reset por software*/
		__HAL_RCC_CLEAR_RESET_FLAGS();
		DataFrame.periodic = 0;
		qDebugMessage("SOFTWARE_RESET\r\n");

		//ContTime = 0;
		//Flash_Write(FLASHMEM.PageAddress + 2,ContTime);
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST)){
		/*Power ON/DOWN*/
		//Flags_globals.flag_ON_WAKEUP_TIME = 1;
		//ContTime = 0;
		//Flash_Write(FLASHMEM.PageAddress + 2,ContTime);
		qDebugMessage("POWER-ON_RESET (POR) / POWER-DOWN_RESET (PDR)\r\n");

		DataFrame.periodic = 0;
		__HAL_RCC_CLEAR_RESET_FLAGS();
	}else {
		/*Variable para reporte por tiempo*/
		DataFrame.periodic = 1;
		qDebugMessage("Wake up Transmision Time\r\n");

	}

}
/*
===================================================================================
		 	 ### Funcion recibe por interrupcion de recepcion
===================================================================================
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	//sigfoxISRRX(&SigfoxModule, UART_RX.Data);
	//qResponseISRHandler(&ResponseObject,rxbyte);

	if(huart->Instance == USART2){
		WSSFM1XRX_ISRRX(&SigfoxModule,UART_RX.Data);  // queda almacenado en  UART_RX.Data;
		HAL_UART_Receive_IT( &huart2,(uint8_t *)&UART_RX.Data,USART_RX_AMOUNT_BYTES);
	}
	if(huart->Instance == USART3){
		qResponseISRHandler(&ResponseObject,UART_RX3.Data);
		if(UART_RX3.Index < sizeof(UART_RX3.Buffer)-1){
			UART_RX3.Buffer[UART_RX3.Index++] = UART_RX3.Data;
			UART_RX3.Buffer[UART_RX3.Index] = '\0';
		}else UART_RX3.Index = 0;
		HAL_UART_Receive_IT(&huart3,(uint8_t *)&UART_RX3.Data,USART_RX_AMOUNT_BYTES);
	}
}

void ProjectInformation(void) {

	PutStringWrapperUart_2(NULL,"\r\n=========================================\r\n");
	PutStringWrapperUart_2(NULL, APP_STRING);
	PutStringWrapperUart_2(NULL, INF_STRING);
	PutStringWrapperUart_2(NULL, REV_STRING);
	PutStringWrapperUart_2(NULL, AUT_STRING);
	PutStringWrapperUart_2(NULL, URL_STRING);
	PutStringWrapperUart_2(NULL,"=========================================\r\n\r\n");
}

