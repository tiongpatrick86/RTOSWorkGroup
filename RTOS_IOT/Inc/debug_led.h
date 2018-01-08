/*
 * debug_led.h
 *
 *  Created on: Jan 7, 2018
 *      Author: pt
 */

#ifndef DEBUG_LED_H_
#define DEBUG_LED_H_

#include "main.h"
#include "stm32l0xx_hal.h"

/*** Configuration Option***********************/
// CONFIG: Need to comment below based on usage

#define USE_CMSIS_FREERTOS
//#define TESTING_IN_RTOS
/**********************************************/


void HeartBeat_LED_Init (GPIO_TypeDef * LED_Port , GPIO_InitTypeDef * GPIO);
void LED_HeartBeat (void const * argument);






#endif /* DEBUG_LED_H_ */
