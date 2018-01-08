/*
 * console.c
 *
 *  Created on: Jan 8, 2018
 *      Author: pt
 */
#include "console.h"

#define SIZEOFBUFFER		0x20


static UART_HandleTypeDef * hConsoleUart;
static uint8_t RXBUFFER[SIZEOFBUFFER];

#ifdef USE_CMSIS_FREERTOS

	#include "cmsis_os.h"
	TaskHandle_t Task_console;
	QueueHandle_t QueueConsole;
	SemaphoreHandle_t MutexConsole;

#endif


static uint8_t* ScanChar (uint8_t* achar, uint8_t c, uint32_t maxlen){
	uint32_t i=0;
	while (i++<=maxlen){
		if (*achar == c){
			return achar;
		}
	}
	return 0;
}


eConsoleRespond _swReply(char * Command ){
	if (strcmp(Command,"HELP")==0)return Console_Help;
	if (strcmp(Command,"BACK")==0)return Console_Back;
	if (strcmp(Command,"EXIT")==0)return Console_Exit;
	return Console_Wait;
}



__weak void _cbTaskConsole ( void *argument ){
//	char *pData = 0;
	TickType_t QueueDelay = portMAX_DELAY;
	uint32_t data = 0;
	char aText[SIZEOFBUFFER];
	char *pText = 0;
	eConsoleRespond prog;

	for (;;){

#ifdef USE_CMSIS_FREERTOS

		xQueueReceive(QueueConsole, &data ,QueueDelay);
		strcpy(aText,(char *)data);
		free((char *)data);
		memset((char *)data, 0x00, strlen((char *)data));

		prog = _swReply(aText);

		if (QueueDelay == portMAX_DELAY) {
				pText = (char *)"\nThe System will now enter the Idle Mode. \nKey In HELP for more Command information.";
				HAL_UART_Transmit(hConsoleUart,(uint8_t*)pText,strlen(pText),50);
				QueueDelay = 0;
			}
		else {
			switch (prog){
			case Console_Help:
				HAL_UART_Transmit(hConsoleUart,(uint8_t*)aText,strlen(aText),100);

				pText = (char *)"\n*EXIT \n*BACK";
				HAL_UART_Transmit(hConsoleUart,(uint8_t*)pText,strlen(pText),50);

				pText = (char *)"\n>>";
				HAL_UART_Transmit(hConsoleUart,(uint8_t*)pText,strlen(pText),50);
				break;

			case Console_Back:
				HAL_UART_Transmit(hConsoleUart,(uint8_t*)aText,strlen(aText),100);

				pText = (char *)"\nBACK command is pressed";
				HAL_UART_Transmit(hConsoleUart,(uint8_t*)pText,strlen(pText),50);

				pText = (char *)"\n>>";
				HAL_UART_Transmit(hConsoleUart,(uint8_t*)pText,strlen(pText),50);
				break;

			case Console_Wait:

				break;

			case Console_Exit:
				pText = (char *)"\nSystem will return to Normal Operation.";
				HAL_UART_Transmit(hConsoleUart,(uint8_t*)pText,strlen(pText),50);

				QueueDelay = portMAX_DELAY;
				break;

			default:
				HAL_UART_Transmit(hConsoleUart,aText,strlen(aText),100);
				break;
			}

		}

//	xQueueReceive(QueueConsole, &data ,QueueDelay);
//	pData = (char *)data;
//	HAL_UART_Transmit(hConsoleUart,(uint8_t*)pData,strlen(pData),50);
//
//	if (QueueDelay == portMAX_DELAY) {
//		pText = (char *)"\nThe System will now enter the Idle Mode. \nKey In HELP for more Command information.";
//		HAL_UART_Transmit(hConsoleUart,(uint8_t*)pText,strlen(pText),50);
//		QueueDelay = 0;
//		free(pData);
//	}
//	else {
//		switch (_swReply(pData)){
//		case Console_Help:
//			pText = (char *)"\n*EXIT \n*BACK";
//			HAL_UART_Transmit(hConsoleUart,(uint8_t*)pText,strlen(pText),50);
//
//			pText = (char *)"\n>>";
//			HAL_UART_Transmit(hConsoleUart,(uint8_t*)pText,strlen(pText),50);
//
//			free(pData);
//			memset(pData,0x00,strlen(pData));
//			break;
//
//		case Console_Back:
//			pText = (char *)"\nBACK command is pressed";
//			HAL_UART_Transmit(hConsoleUart,(uint8_t*)pText,strlen(pText),50);
//
//			pText = (char *)"\n>>";
//			HAL_UART_Transmit(hConsoleUart,(uint8_t*)pText,strlen(pText),50);
//
//			free(pData);
//			memset(pData,0x00,strlen(pData));
//			break;
//
//		case Console_Exit:
//			pText = (char *)"\nSystem will return to Normal Operation.";
//			HAL_UART_Transmit(hConsoleUart,(uint8_t*)pText,strlen(pText),50);
//
//			QueueDelay = portMAX_DELAY;
//
//			free(pData);
//			memset(pData,0x00,strlen(pData));
//			break;
//
//		case Console_Wait:
//			break;
//
//		default:
//			break;
//		}
//	}
#endif
	}
}


void Console_link (UART_HandleTypeDef *serial){

	hConsoleUart = serial;

	memset(RXBUFFER,(uint8_t)0x00, SIZEOFBUFFER);

	if (HAL_UART_Receive_IT(hConsoleUart,&RXBUFFER[0],SIZEOFBUFFER)!=HAL_OK){
		while(1);
	}

	HAL_NVIC_EnableIRQ(USART2_IRQn);
	HAL_NVIC_SetPriority(USART2_IRQn,2,0);

#ifdef USE_CMSIS_FREERTOS
	xTaskCreate(_cbTaskConsole, "Console_Thread", 128, NULL, 4, &Task_console);
	QueueConsole = xQueueCreate(1, sizeof(uint8_t *));
	MutexConsole = xSemaphoreCreateMutex();
#endif

}


void Console_NVIC (UART_HandleTypeDef *serial){

#ifdef USE_CMSIS_FREERTOS
	uint8_t* pRxBuff = serial->pRxBuffPtr;
	uint8_t seq = (serial->RxXferSize - serial->RxXferCount);
	uint32_t pAddress = 0;

	if (*(pRxBuff-1)== 0x0D){

		*(pRxBuff-1) = 0;
		pRxBuff = (uint8_t *)malloc(seq + 1);

		serial->pRxBuffPtr -= seq;
		serial->RxXferCount = serial->RxXferSize;

		strcpy((char *)pRxBuff, (char *)serial->pRxBuffPtr);

		pAddress = (uint32_t)pRxBuff;
		xQueueSendFromISR(QueueConsole,&pAddress,0);
		memset(RXBUFFER,(uint8_t)0x00, SIZEOFBUFFER);
	}
	else if(serial->RxXferCount == 0){

		serial->pRxBuffPtr -= seq;
		serial->RxXferCount = serial->RxXferSize;

		memset(RXBUFFER,(uint8_t)0x00, SIZEOFBUFFER);
	}
	else {

	}

#else

	uint8_t* pRxBuff = serial->pRxBuffPtr;
	uint8_t seq = 0;

	seq = (serial->RxXferSize - serial->RxXferCount);

	if (*(pRxBuff-1)=='\n'){
		serial->pRxBuffPtr -= seq;
		serial->RxXferCount = serial->RxXferSize;
		HAL_UART_Transmit(serial,serial->pRxBuffPtr,seq,100);
		memset(RXBUFFER,(uint8_t)0x00, SIZEOFBUFFER);
	}
	else if(serial->RxXferCount == 0){
		serial->pRxBuffPtr -= seq;
		serial->RxXferCount = serial->RxXferSize;
		memset(RXBUFFER,(uint8_t)0x00, SIZEOFBUFFER);
	}
	else {

	}
#endif
}



