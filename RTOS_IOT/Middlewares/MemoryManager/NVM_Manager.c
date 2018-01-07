#include "NVM_Manager.h"

#define EEPROM_START_ADDRESS				(uint32_t *)0x08080000
#define EEPROM_END_ADDRESS					(uint32_t *)0x080807CF  // only have 2kBytes

#define EEPROM_FIFO_DATASTORE_ADDRESS		(uint32_t *)0x08080000	// EEPROM

#define BUTTON_TIMEOUT						(uint32_t)0x1388		// 5 seconds timeout


extern osSemaphoreId SemaBlockClearEEPROM;

static int32_t timeout = 0;
static EEPROM_FIFO *FIFO_DATA = (EEPROM_FIFO *)EEPROM_FIFO_DATASTORE_ADDRESS;



osThreadId Task_ManagEeprom;
osMutexId MutexEeprom;
osSemaphoreId SemaEepromManager;



static void EEPROM_Clean(void){
	volatile uint32_t*  Ptr_address=0;
	Ptr_address = EEPROM_START_ADDRESS;
	HAL_FLASHEx_DATAEEPROM_Unlock();

	while (Ptr_address<=EEPROM_END_ADDRESS){
		HAL_FLASHEx_DATAEEPROM_Erase((uint32_t)Ptr_address);
		Ptr_address++;
	}

	HAL_FLASHEx_DATAEEPROM_Lock();
}

static void EEPROM_Read (void){

}


static void EEPROM_Write (enum eEEPROM_METHOD method, uint32_t Address, uint32_t* Data, uint32_t len){
	if (method == CONTINUE_WITH_FIFO){

	}
	else { //USE_ADDRESS

	}
}

/*
 * The task 'task_EepromManager' will automatically goes into the block status and waiting for the semaphore 'SemaEepromManager'
 * to be Released by NVIC interrupt. The NVIC interrupt required to be pressed for 'BUTTON_TIMEOUT' interval.
 *
 * If task is activated the LED is blink for 3 times and then will wait for MUTEX 'MutexEeprom' to be released before flushing out
 * all the data from the eeprom.
 */


void task_EepromManager ( void const* argument ){
	uint8_t i;

	for (;;){

		osSemaphoreWait(SemaEepromManager,osWaitForever);

		for (i=0; i<3 ; i++){
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_SET);
				HAL_Delay(100);
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_RESET);
				HAL_Delay(100);
			}
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_SET);

			//TODO: Implement Mutex before EEPROM Clean is carried out
			osMutexWait(MutexEeprom, osWaitForever);

			EEPROM_Clean();

			osMutexRelease(MutexEeprom);
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_RESET);

	}
}


void EEPROM_Initialize ( void ){

	osThreadDef(EepromManager, task_EepromManager, osPriorityAboveNormal, 0 , 128);
	Task_ManagEeprom = osThreadCreate(osThread(EepromManager),NULL);

	osSemaphoreDef(SemaDefEeprom);
	SemaEepromManager = osSemaphoreCreate(osSemaphore(SemaDefEeprom), 1);
	osSemaphoreWait(SemaEepromManager,osWaitForever);

	osMutexDef(MutexEeprom);
	MutexEeprom = osMutexCreate(osMutex(MutexEeprom));

}

/**************************************************
 *
 *   Interrupt routine to release the Semaphore
 *
 *
 *
 * **************************************************/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	if (timeout!= 0){
		timeout = osKernelSysTick() - timeout;
		if (timeout >= BUTTON_TIMEOUT){
			osSemaphoreRelease(SemaEepromManager);
		}
		timeout = 0;
	}
	else{
		timeout = osKernelSysTick();
	}
}
