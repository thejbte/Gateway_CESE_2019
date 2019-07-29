/*
 * Task.c
 *
 *  Created on: 1/02/2019
 *      Author: julian
 */


#include "Task/Task.h"
#include "Drivers_Hd/wssfm1xrx.h"

void Leds_Callback(qEvent_t e){
	IndicatorBlink(Color.TimeBlink,Color.OneColor);  // 300 mS
}


void IdleTask_Callback(qEvent_t e){


	//	PrintStringVar(&huart2,(uint8_t*) "status = %d ", SigFoxData.WilsolService_Status);
}

void UplinkDispatcher_Callback(qEvent_t e){
	static WSSFM1XRX_Return_t ret;
	qCoroutineBegin{

		if(e->Trigger == byRBufferCount && SigFoxData.ReadyToUplink){
			SigFoxData.WilsolService_Status = WSSFM1XRX_STATUS_CHK_CHANNELS;
			PrintStringVar(&hlpuart1,(uint8_t*) "%d " , SigFoxData.WilsolService_Status);
			qCoroutineWaitUntil( (ret = WSSFM1XRX_CheckChannels(&SigfoxModule, WAIT ))  == WSSFM1XRX_CHANN_OK || ret == WSSFM1XRX_CHANN_NO_OK );


			if(  ret == WSSFM1XRX_CHANN_NO_OK   ){
				qCoroutineWaitUntil( WSSFM1XRX_OK_RESPONSE ==  WSSFM1XRX_ResetChannels(&SigfoxModule,WAIT) );  /*Se daña con 0.01 en task pero solo con el at$rc*/ //WSSFM1XRX_Wait_Block
				SigFoxData.WilsolService_Status = WSSFM1XRX_STATUS_RST_CHANNELS;
				PrintStringVar(&hlpuart1,(uint8_t*) "%d ", SigFoxData.WilsolService_Status);
			}

			SigFoxData.WilsolService_Status = WSSFM1XRX_STATUS_SEND_MESSAGE;
			PrintStringVar(&hlpuart1,(uint8_t*) "%d ", SigFoxData.WilsolService_Status);



			qCoroutineWaitUntil( WSSFM1XRX_OK_RESPONSE == WSSFM1XRX_SendMessage(&SigfoxModule,WAIT,qRBufferGetFront(e->EventData) ,BufferTxFrame,MAX_SIZE_IBUTTON_DATA,SigfoxModule.DownLink) );


			SigFoxData.WilsolService_Status = WSSFM1XRX_STATUS_SENT_MESSAGE;
			qRBufferRemoveFront(e->EventData);
			PrintStringVar(&hlpuart1,(uint8_t*) "%d\r\n", SigFoxData.WilsolService_Status);
		}
	}qCoroutineEnd;
}

void WisolService_Callback(qEvent_t e){
	static WSSFM1XRX_Return_t ret;
	static qSTimer_t timeout;

	qCoroutineBegin{

		HAL_GPIO_WritePin(GPIOC, Gpio4_Bus1_Shutdown_Pin, GPIO_PIN_SET);
		qDebugMessage("Coroutine from Beginning Sigfox");
		qDebugMessage("Coroutine from Beginning Lora");
		SigFoxData.WilsolService_Status = WSSFM1XRX_STATUS_WKUP;
		PrintStringVar(&hlpuart1,(uint8_t*) "Status = %d ", SigFoxData.WilsolService_Status);
		ret = WSSFM1XRX_WakeUP(&SigfoxModule,WSSFM1XRX_Wait_Block);

		qSTimerSet(&timeout, 1.0);
		qCoroutineWaitUntil( qSTimerExpired(&timeout) );
		SigFoxData.WilsolService_Status = WSSFM1XRX_STATUS_CHK_MODULE;
		PrintStringVar(&hlpuart1,(uint8_t*) "%d ", SigFoxData.WilsolService_Status);
		qCoroutineWaitUntil( ( WSSFM1XRX_OK_RESPONSE == (ret = WSSFM1XRX_CheckModule(&SigfoxModule, WAIT)) )  || (WSSFM1XRX_MAX_RETRIES_REACHED == ret)  );
		if(WSSFM1XRX_MAX_RETRIES_REACHED  == ret ){
			WSSFM1XRX_ResetModule(&SigfoxModule,WSSFM1XRX_Wait_Block);
			qCoroutineRestart;
		}
		SigFoxData.WilsolService_Status = WSSFM1XRX_STATUS_GET_VOLTAGE;
		PrintStringVar(&hlpuart1,(uint8_t*) "%d ", SigFoxData.WilsolService_Status);
		qCoroutineWaitUntil( WSSFM1XRX_OK_RESPONSE == WSSFM1XRX_GetVoltage(&SigfoxModule,WAIT,&SigFoxData.VBatt) );

		SigFoxData.WilsolService_Status = WSSFM1XRX_STATUS_CHANGE_FREQ_UL;
		PrintStringVar(&hlpuart1,(uint8_t*) "%d ", SigFoxData.WilsolService_Status);
		qCoroutineWaitUntil( WSSFM1XRX_OK_RESPONSE == WSSFM1XRX_ChangeFrequencyUL(&SigfoxModule,WAIT,WSSFM1XRX_RCZ4)   );

		SigFoxData.WilsolService_Status = WSSFM1XRX_STATUS_SAVE_PARM;
		PrintStringVar(&hlpuart1,(uint8_t*) "%d ", SigFoxData.WilsolService_Status);
		qCoroutineWaitUntil( WSSFM1XRX_OK_RESPONSE == WSSFM1XRX_SaveParameters(&SigfoxModule,WAIT) );

		SigFoxData.WilsolService_Status = WSSFM1XRX_STATUS_GET_ID;
		PrintStringVar(&hlpuart1,(uint8_t*) "%d ", SigFoxData.WilsolService_Status);
		qCoroutineWaitUntil( WSSFM1XRX_OK_RESPONSE == WSSFM1XRX_GetID(&SigfoxModule,WAIT,SigFoxData.ID) );

		SigFoxData.WilsolService_Status = WSSFM1XRX_STATUS_GET_PAC;
		PrintStringVar(&hlpuart1,(uint8_t*) "%d ", SigFoxData.WilsolService_Status);
		qCoroutineWaitUntil( WSSFM1XRX_OK_RESPONSE == WSSFM1XRX_GetPAC(&SigfoxModule,WAIT,SigFoxData.PAC) );

		SigFoxData.WilsolService_Status = WSSFM1XRX_STATUS_MODULE_INITIALIZED;
		PrintStringVar(&hlpuart1,(uint8_t*) "%d ", SigFoxData.WilsolService_Status);
		//qCoroutineWaitUntil( SigFoxData.WilsolService_Status == WSSFM1XRX_STATUS_SEND_MESSAGE ); //WSSFM1XRX_STATUS_SEND_MESSAGE  );  WSSFM1XRX_STATUS_MODULE_INITIALIZED

		SigFoxData.ReadyToUplink = 1;
		//qDebugMessage( SigFoxData.ID);
		qCoroutineWaitUntil( SigFoxData.WilsolService_Status == WSSFM1XRX_STATUS_IDLE  );

	}qCoroutineEnd;
}

void LoRaWANService_Callback(qEvent_t e){
	static char BufferAux[100];
	static uint8_t contRetry,i=0;

	qCRPosition_t state;
	qCoroutineBegin{
		HAL_GPIO_WritePin(Sigfox_Rst_GPIO_Port, Sigfox_Rst_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(Sigfox_shutdown_GPIO_Port, Sigfox_shutdown_Pin, GPIO_PIN_SET);
		qDebugMessage("Coroutine from Beginning Lora");
		//qTraceMessage("Task_ServiceConfigBG96_Callback");
		//qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"APP RDY\r\n",4)));


		/*
		mac set devaddr 26021CF3
		mac set appeui 70B3D57ED001E6F9
		mac set deveui 0004A30B00264D28
		mac set nwkskey C42DD69087BFA056D282134269E49422
		mac set appskey EFF299DC7EF67B8EF02509FA0BFF6005
		mac set ar off
		mac set sync 34
		mac set retx 3
		*/


		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"sys reset\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"RN2903\r\n",0)) );

		//qPrintString(PutCharWrapperUart_1,NULL,UART_RX3.Buffer);

		qCoroutinePositionGet(state);
		qSTimerSet(&Timeout,2.0);
		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"mac set devaddr 26021A71\r\n");

		qDebugMessage("mac set devaddr 26021A71\r\n");

		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"ok\r\n",4))||(qSTimerExpired(&Timeout)));

		//qPrintString(PutCharWrapperUart_1,NULL,UART_RX3.Buffer);

		if(qSTimerExpired(&Timeout)){
			contRetry++;
			if(contRetry >=5 ){
				contRetry = 0;
				qCoroutineRestart;
			}else qCoroutinePositionRestore(state);
		}

		qCoroutinePositionGet(state);
		qSTimerSet(&Timeout,2.0);
		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"mac set appeui 70B3D57ED001E6F9\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"ok\r\n",4))||(qSTimerExpired(&Timeout)));
		if(qSTimerExpired(&Timeout)){
			contRetry++;
			if(contRetry >=5 ){
				contRetry = 0;
				qCoroutineRestart;
			}else qCoroutinePositionRestore(state);
		}

		qCoroutinePositionGet(state);
		qSTimerSet(&Timeout,2.0);
		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"mac set deveui 0004A30B00264D28\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"ok\r\n",4))||(qSTimerExpired(&Timeout)));
		if(qSTimerExpired(&Timeout)){
			contRetry++;
			if(contRetry >=5 ){
				contRetry = 0;
				qCoroutineRestart;
			}else qCoroutinePositionRestore(state);
		}

		qCoroutinePositionGet(state);
		qSTimerSet(&Timeout,2.0);
		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"mac set nwkskey C4B2C1017D1F621B2866535BF13FDA29\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"ok\r\n",4))||(qSTimerExpired(&Timeout)));
		if(qSTimerExpired(&Timeout)){
			contRetry++;
			if(contRetry >=5 ){
				contRetry = 0;
				qCoroutineRestart;
			}else qCoroutinePositionRestore(state);
		}

		qCoroutinePositionGet(state);
		qSTimerSet(&Timeout,2.0);
		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"mac set appskey 2683DB00F5C82D7F554E89502C9A3BB9\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"ok\r\n",4))||(qSTimerExpired(&Timeout)));
		if(qSTimerExpired(&Timeout)){
			contRetry++;
			if(contRetry >=5 ){
				contRetry = 0;
				qCoroutineRestart;
			}else qCoroutinePositionRestore(state);
		}

		qCoroutinePositionGet(state);
		qSTimerSet(&Timeout,2.0);
		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"mac set ar off\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"ok\r\n",4))||(qSTimerExpired(&Timeout)));
		if(qSTimerExpired(&Timeout)){
			contRetry++;
			if(contRetry >=5 ){
				contRetry = 0;
				qCoroutineRestart;
			}else qCoroutinePositionRestore(state);
		}

		qCoroutinePositionGet(state);
		qSTimerSet(&Timeout,2.0);
		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"mac set sync 34\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"ok\r\n",4))||(qSTimerExpired(&Timeout)));
		if(qSTimerExpired(&Timeout)){
			contRetry++;
			if(contRetry >=5 ){
				contRetry = 0;
				qCoroutineRestart;
			}else qCoroutinePositionRestore(state);
		}

		qCoroutinePositionGet(state);
		qSTimerSet(&Timeout,2.0);
		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"mac set retx 3\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"ok\r\n",4))||(qSTimerExpired(&Timeout)));
		if(qSTimerExpired(&Timeout)){
			contRetry++;
			if(contRetry >=5 ){
				contRetry = 0;
				qCoroutineRestart;
			}else qCoroutinePositionRestore(state);
		}


		 	 i=0;
			qCoroutinePositionGet(state);
			qSTimerSet(&Timeout,2.0);
			sprintf((void*)BufferAux,"mac set ch status %d ",i);
			qPrintString(UART_LORA,NULL,BufferAux);
			qPrintString(UART_LORA,NULL,"off\r\n");
			//qCoroutineYield;
			qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"ok\r\n",4))||(qSTimerExpired(&Timeout)));
			if( i<71 ){
				if(!qSTimerExpired(&Timeout)){
					i++;
					qCoroutinePositionRestore(state);
				}else {
					contRetry++;
						if(contRetry >=5 ){
							contRetry = 0;
							qCoroutineRestart;
						}else qCoroutinePositionRestore(state);
				}
			}


		 	 i=8;
			qCoroutinePositionGet(state);
			qSTimerSet(&Timeout,2.0);
			sprintf((void*)BufferAux,"mac set ch status %d ",i);
			qPrintString(UART_LORA,NULL,BufferAux);
			qPrintString(UART_LORA,NULL,"off\r\n");
			//qCoroutineYield;
			qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"ok\r\n",4))||(qSTimerExpired(&Timeout)));
			if( i<15 ){
				if(!qSTimerExpired(&Timeout)){
					i++;
					qCoroutinePositionRestore(state);
				}else {
					contRetry++;
						if(contRetry >=5 ){
							contRetry = 0;
							qCoroutineRestart;
						}else qCoroutinePositionRestore(state);
				}
			}


		qCoroutinePositionGet(state);
		qSTimerSet(&Timeout,2.0);
		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"mac set ch status 8 on\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"ok\r\n",4))||(qSTimerExpired(&Timeout)));
		if(qSTimerExpired(&Timeout)){
			contRetry++;
			if(contRetry >=5 ){
				contRetry = 0;
				qCoroutineRestart;
			}else qCoroutinePositionRestore(state);
		}

		qCoroutinePositionGet(state);
		qSTimerSet(&Timeout,2.0);
		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"mac save\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"ok\r\n",4))||(qSTimerExpired(&Timeout)));
		if(qSTimerExpired(&Timeout)){
			contRetry++;
			if(contRetry >=5 ){
				contRetry = 0;
				qCoroutineRestart;
			}else qCoroutinePositionRestore(state);
		}

		qCoroutinePositionGet(state);
		qSTimerSet(&Timeout,2.0);
		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"mac save\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"ok\r\n",4))||(qSTimerExpired(&Timeout)));
		if(qSTimerExpired(&Timeout)){
			contRetry++;
			if(contRetry >=5 ){
				contRetry = 0;
				qCoroutineRestart;
			}else qCoroutinePositionRestore(state);
		}

		qCoroutinePositionGet(state);
		qSTimerSet(&Timeout,2.0);
		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"mac join abp\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"accepted\r\n",4))||(qSTimerExpired(&Timeout)));
		if(qSTimerExpired(&Timeout)){
			contRetry++;
			if(contRetry >=5 ){
				contRetry = 0;
				qCoroutineRestart;
			}else qCoroutinePositionRestore(state);
		}

		qCoroutinePositionGet(state);
		qSTimerSet(&Timeout,20.0);
		memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
		qPrintString(UART_LORA,NULL,"mac tx uncnf 1 0000000009c7\r\n");
		qDebugMessage("mac tx uncnf 1 0000000009c7\r\n");
		//keys_not_init  or invalid_param
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"accept\r\n",4))||(qSTimerExpired(&Timeout)));
		if(qSTimerExpired(&Timeout)){
			contRetry++;
			if(contRetry >=5 ){
				contRetry = 0;
				memset(UART_RX3.Buffer,0,sizeof(UART_RX3.Buffer));
				qCoroutineRestart;
			}else qCoroutinePositionRestore(state);
		}
		LoRaReadyToUplink = 1;
		qCoroutineWaitUntil( SigFoxData.WilsolService_Status == WSSFM1XRX_STATUS_IDLE  );
		//qPrintString(UART_BG96,NULL,BG96_Data.Frame);
	}qCoroutineEnd;



}
