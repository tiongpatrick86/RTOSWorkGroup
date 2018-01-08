/*
 * console.h
 *
 *  Created on: Jan 8, 2018
 *      Author: pt
 */
#include "main.h"
#include "cmsis_os.h"
#include "stm32l0xx_hal.h"
#include <string.h>

#define USE_CMSIS_FREERTOS



#ifdef USE_CMSIS_FREERTOS

#endif


typedef enum {
	Console_Help,
	Console_Back,
	Console_Wait,
	Console_Exit,
}eConsoleRespond;


void Console_link (UART_HandleTypeDef *serial);
void Console_NVIC (UART_HandleTypeDef *serial);
