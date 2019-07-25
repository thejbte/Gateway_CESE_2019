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
		qDebugMessage("Coroutine from Beginning");
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

		qCoroutineWaitUntil( SigFoxData.WilsolService_Status == WSSFM1XRX_STATUS_IDLE  );

	}qCoroutineEnd;
}

void LoRaWANService_Callback(qEvent_t e){
	static char BufferAux[100];
	qCoroutineBegin{
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

		qSTimerSet(&Timeout,2.0);
		//memset(BG96_Data.Frame,0,sizeof(BG96_Data.Frame));
		qPrintString(UART_DEBUG,NULL,"mac set devaddr 26021CF3\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"OK\r\n",4))||(qSTimerExpired(&Timeout)));

		qSTimerSet(&Timeout,2.0);
		//memset(BG96_Data.Frame,0,sizeof(BG96_Data.Frame));
		qPrintString(UART_DEBUG,NULL,"mac set appeui 70B3D57ED001E6F9\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"OK\r\n",4))||(qSTimerExpired(&Timeout)));

		qSTimerSet(&Timeout,2.0);
		//memset(BG96_Data.Frame,0,sizeof(BG96_Data.Frame));
		qPrintString(UART_DEBUG,NULL,"mac set deveui 0004A30B00264D28\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"OK\r\n",4))||(qSTimerExpired(&Timeout)));

		qSTimerSet(&Timeout,2.0);
		//memset(BG96_Data.Frame,0,sizeof(BG96_Data.Frame));
		qPrintString(UART_DEBUG,NULL,"mac set nwkskey C42DD69087BFA056D282134269E49422\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"OK\r\n",4))||(qSTimerExpired(&Timeout)));

		qSTimerSet(&Timeout,2.0);
		//memset(BG96_Data.Frame,0,sizeof(BG96_Data.Frame));
		qPrintString(UART_DEBUG,NULL,"mac set appskey EFF299DC7EF67B8EF02509FA0BFF6005\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"OK\r\n",4))||(qSTimerExpired(&Timeout)));

		qSTimerSet(&Timeout,2.0);
		//memset(BG96_Data.Frame,0,sizeof(BG96_Data.Frame));
		qPrintString(UART_DEBUG,NULL,"mac set ar off\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"OK\r\n",4))||(qSTimerExpired(&Timeout)));

		qSTimerSet(&Timeout,2.0);
		//memset(BG96_Data.Frame,0,sizeof(BG96_Data.Frame));
		qPrintString(UART_DEBUG,NULL,"mac set sync 34\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"OK\r\n",4))||(qSTimerExpired(&Timeout)));

		qSTimerSet(&Timeout,2.0);
		//memset(BG96_Data.Frame,0,sizeof(BG96_Data.Frame));
		qPrintString(UART_DEBUG,NULL,"mac set retx 3\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"OK\r\n",4))||(qSTimerExpired(&Timeout)));


		for(int i=0; i< 72 ; i++){
			//qSTimerSet(&Timeout,0.2);
			qSTimerSet(&Timeout,2.0);
			sprintf((void*)BufferAux,"mac set ch status %d ",i);
			qPrintString(UART_DEBUG,NULL,BufferAux);
			qPrintString(UART_DEBUG,NULL,"off\r\n");
			qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"OK\r\n",4))||(qSTimerExpired(&Timeout)));
		}
		for(int i=8; i< 15 ; i++){
			qSTimerSet(&Timeout,2.0);
			sprintf((void*)BufferAux,"mac set ch status %d ",i);
			qPrintString(UART_DEBUG,NULL,BufferAux);
			qPrintString(UART_DEBUG,NULL,"on\r\n");
			qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"OK\r\n",4))||(qSTimerExpired(&Timeout)));
		}

		qSTimerSet(&Timeout,2.0);
		qPrintString(UART_DEBUG,NULL,"mac set ch status 65 on\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"OK\r\n",4))||(qSTimerExpired(&Timeout)));

		qSTimerSet(&Timeout,2.0);
		//memset(BG96_Data.Frame,0,sizeof(BG96_Data.Frame));
		qPrintString(UART_DEBUG,NULL,"mac save\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"OK\r\n",4))||(qSTimerExpired(&Timeout)));

		qSTimerSet(&Timeout,2.0);
		//memset(BG96_Data.Frame,0,sizeof(BG96_Data.Frame));
		qPrintString(UART_DEBUG,NULL,"mac save\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"OK\r\n",4))||(qSTimerExpired(&Timeout)));

		qSTimerSet(&Timeout,2.0);
		//memset(BG96_Data.Frame,0,sizeof(BG96_Data.Frame));
		qPrintString(UART_DEBUG,NULL,"mac join abp\r\n");
		qCoroutineWaitUntil((qResponseReceived(&ResponseObject,"accepted\r\n",4))||(qSTimerExpired(&Timeout)));

		//keys_not_init  or invalid_param


		//qPrintString(UART_BG96,NULL,BG96_Data.Frame);
	}qCoroutineEnd;



}