/*
 * NVM_Manager.h
 *
 *  Created on: Jan 6, 2018
 *      Author: pt
 */

#ifndef NVM_MANAGER_H_
#define NVM_MANAGER_H_

#include "main.h"
#include "cmsis_os.h"
#include "stm32l0xx_hal.h"

typedef struct {
	uint32_t Cons;
	uint32_t Produce;
	uint32_t Flag;
}EEPROM_FIFO;

enum eEEPROM_METHOD {
	CONTINUE_WITH_FIFO,
	USE_ADDRESS,
};



void EEPROM_Initialize ( void );



#endif /* NVM_MANAGER_H_ */
