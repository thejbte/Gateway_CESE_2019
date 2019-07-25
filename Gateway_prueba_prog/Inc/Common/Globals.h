/*
 * Globals.h
 *
 *  Created on: 30/01/2019
 *      Author: julian
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_


#include <Drivers_Hd/Debounce.h>
//#include <Hardware_Profile/RTC_Config.h>
//#include <Hardware_Profile/Uart.h>
//#include "../Inc/Hardware_Profile/GPIO_Config.h"
//#include "../Inc/Hardware_Profile/Internal_Flash.h"
#include "main.h"
//#include "stm32f0xx.h"

#include "stdio.h"
#include "string.h"
#include "stdint.h"

#include "../Inc/Kernel/QuarkTS.h"
#include "../Inc/Task/Task.h"
#include "../Inc/State/State.h"

//#include "../Inc/SystemClock_Config.h"
#include "../Inc/Common/Event.h"


#include "../Inc/Drivers_Hd/wssfm1xrx.h"


#define WAIT WSSFM1XRX_Wait_NonBlock


/*Definition */
//#define Once_Bytes
#define Doce_Bytes


#define APP_STRING  QUARKTS_CAPTION "-" "WisolDriver: "WSSFM1XRX_VERSION  "-" __DATE__ "-" __TIME__ "\r\n"
#define INF_STRING  "Project: Gateway IoT CESE 2019\r\n"
#define REV_STRING  "REV: 1.0..0\r\n"
#define AUT_STRING  "Developer: [Julian Bustamante N]\r\n"
#define URL_STRING  "Contact: https://tecrea.com.co \r\n"


/*Definition Others*/
//#define MAX_BUFFER_SIZE
#define VERBOSE


#define RC_4

/*Wrap */
#define iButton_Data DataFrame


/*Numero de pulsaciones*/
#define ONEPRESS	1
#define TWOPRESS	2
#define THREEPRESS	3

#define MAX_NUM_PULSATION 3

#define MAX_WAIT_TIME_TO_PRESS_BUTTON 5.0

#define MAX_WAITING_TIME_TO_LED_BLINK	2.0

#define TIMEBLINKLED 0.3

/*Umbral alarma  Voltaje bateria */
#define ALARMA_LOW_BATTERY	2700

/*DECIMAL*/
#define BASE_DECIMAL	10

/*MAX_SIZE_IBUTTON_DATA sendpayload*/

#ifdef _11_BYTES
#define MAX_SIZE_IBUTTON_DATA	11
#else
#define	MAX_SIZE_IBUTTON_DATA	12
#endif





/** Error */
#define WRAPER_ERR_OK	0X00U /* OK*/

/** USART 1 */
#define USART_TIMEOUT		500
#define USART_RX_AMOUNT_BYTES	1
#define USART_TX_AMOUNT_BYTES	1

/** *UASRT2  */
#define UART_DEBUG PutCharWrapperUart_2

extern volatile qRespHandler_t ResponseObject;

void PrintStringVar(UART_HandleTypeDef *huart,uint8_t *String, uint16_t  Variable);
void PrintString(UART_HandleTypeDef *huart,uint8_t *pData);
void PutCharWrapperUart_1(void *sp, const char c);
void PutCharWrapperUart_2(void *sp, const char c);
void PutStringWrapperUart_2(void *sp, const char *s);
/***************Scheduler*****************************/
extern qTask_t Task_ApplicationFSM, Task_UplinkDispatcher, Task_Wisol_Service, Task_Leds;
qSM_t StateMachine_ApplicationFSM;
extern  qSTimer_t Timeout;
extern qRBuffer_t SigFox_UplinkQueue;


extern qIOEdgeCheck_t InputCheck;
extern qIONode_t InputButton;
/********************Other Variables ***********************/
extern volatile uint8_t wakeup;
extern volatile uint8_t FlagCont;
extern uint8_t ReportTimeHour;
extern uint8_t ReportTimeMinute;
extern uint8_t ReportTimeSecond;
extern uint8_t DownLink;

extern UART_HandleTypeDef hlpuart1;

extern UART_HandleTypeDef huart2;


/** Buffer Rx Frame*/
extern char BufferRxFrame[WSSFM1XRX_BUFF_RX_FRAME_LENGTH];

/** Buffer Tx Frame*/
extern char BufferTxFrame[WSSFM1XRX_BUFF_TX_FRAME_LENGTH];


extern ADC_HandleTypeDef hadc1;




/*Flash memory*/
extern FLASH_EraseInitTypeDef FLASHMEM;





#define InitMemoryAddressReserved	0x08007C00
/*******************Sigfox Library**************************/
extern  WSSFM1XRXConfig_t SigfoxModule;

/*
 ** ===================================================================
 ** 						Wisol service data
 ** ===================================================================
 */

typedef struct {
	union{
		uint8_t Payload[12];
		WSSFM1XRX_FreqUL_t Frequency;
	};
	uint8_t Request;
}SigfoxServiceRequest_t;


typedef enum{
	REQUEST_SEND_PAYLOAD = 0,
	REQUEST_CHANGE_FREQUENCY
}Request_Service_t;

typedef struct{
	char ID[12];
	char PAC[22];
	uint16_t VBatt;
	WSSFM1XRX_Service_Status_t WilsolService_Status;
	uint8_t ReadyToUplink;
}SigFox_Info_t;

extern SigFox_Info_t SigFoxData;


/*Others*/
extern uint8_t FlagChangeFrequencyPulsations;

typedef struct {
uint8_t Buffer[25];
uint8_t Index;
uint8_t Data;
uint8_t Ready;
uint8_t Downlink;
}UART_BufferData_t;
extern UART_BufferData_t UART_RX;
extern UART_BufferData_t UART_RX3;

void RSTCtrl_Sigfox(uint8_t sValue);
void RST2Ctrl_Sigfox(uint8_t sValue);
void UART_SIGFOX_TX_STM(void * Sp, char c); /*Wrapper function Tx*/
unsigned char UART_SIGFOX_RX_STM( unsigned char * Chr); /*Wrapper function Rx*/


/***************Debounce*****************************/
typedef struct {
	uint8_t State;
}Debounce_Flag_t;
extern volatile Debounce_Flag_t Debounce_Flag;
extern DebounceData_t DebounceData;

/**/
extern uint8_t ContTime;



typedef struct {
	unsigned char OneColor;  /* 'R' 'G' 'B' */
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t ContBlink;
	float TimeBlink;

}ColorRGB_t;

extern ColorRGB_t Color;


#define DAYS_KEEP_ALIVE		7

/**************************** Data Sigfox *****************************/
typedef struct {


#ifdef Once_Bytes
	union {
		uint8_t Others[3];
		struct {
			unsigned ADC_12 :12;   /*Canal AN 12 bits*/
			unsigned DUMMY	:12; 	/*N/A*/
		};
		struct {
			unsigned ADC_0:8;	/*Canal AN 8 bits*/
			unsigned ADC_1:8;	/*Canal AN 8 bits*/
			unsigned DI:1; 						// DI  LSB en el orden que lo coloque ac�  DI, BattLow, Type  hacia abajo
			unsigned BattLow:1;
			unsigned Type:5;
			unsigned periodic:1;
		};
	}Once_Bytes;
#endif

#ifdef Doce_Bytes
	union {
		uint8_t Others[4];

		/** Si solo es un canal de 12 bi*/
		struct {
			unsigned ADC_12 :12;	/*Canal AN 12 bits*/
			unsigned DUMMY	:20;	/*N/A*/
		};
		struct {
			unsigned ADC_0:12;		/*Canal AN 12 bits*/
			unsigned ADC_1:12;		/*Canal AN 12 bits*/
			unsigned DI:1; 						// DI  LSB en el orden que lo coloque ac�  DI, BattLow, Type  hacia abajo
			unsigned BattLow:1;
			unsigned Type:5;
			unsigned periodic:1;
		};
	}Doce_Bytes;
	float Longitud;
	float Latitud;

#endif
}DataFrame_t;

extern DataFrame_t DataFrame;

/**************************Flags globals*******************************************************/

typedef struct {
	union{
		uint32_t Flags;
		struct{
			unsigned GPS_NO_ADQUISITION  :1;
			unsigned GPSAttached		 :1;
			unsigned estado_actual		 :1;
			unsigned flag_FirstEntry     :1;
			unsigned System_Init	     :1;
			unsigned flag_ON_WAKEUP_TIME :1;
			unsigned Flag_button         :1;
			unsigned FLAG_Coma_OK        :1;
			unsigned flag_nibble         :1;
			unsigned DL_Power_ON		 :1;
			unsigned msgADC		         :1;
			unsigned msgGPS			     :1;
			unsigned CoordInside		 :1;
			unsigned Flag_Encendido_Moto :1;
			unsigned promedioTemp		 :1;
			unsigned Click               :8;
			unsigned Debounce			 :8;
			unsigned nada	             :1;
		};
	};
}Flags_t;

extern volatile Flags_t Flags_globals;

/**************************tipos de datos a enviar*******************************************************/

typedef enum{
	iButton,
	ButtonFirstPress=10,
	ButtonSecondPress=11,
	ButtonThirdPress=12,
	Event_battery_cuted_moto=17,
	Event_off_on_reed_switch=25,
	Event_on_off_reed_switch=26,
	Event_off_reed_switch=27,
	Event_on_reed_switch=28,
}Types_t;

typedef struct{
	Types_t xtype;
}tipo_t;
extern tipo_t xtypes;
/*********************************************************************************/
WSSFM1XRX_DL_Return_t DiscrimateFrameType(WSSFM1XRXConfig_t *obj);

/** Led segun Pulsaciones */
void IndicatorBlink(float t_ms,char  color);
GPIO_PinState LedColor(char  color,GPIO_PinState state );
/*Get source interrupt*/
void get_system_reset_cause(void);
extern uint8_t mQueue_Stack[3*sizeof(SigfoxServiceRequest_t)];

void ProjectInformation(void);

#endif /* GLOBALS_H_ */


