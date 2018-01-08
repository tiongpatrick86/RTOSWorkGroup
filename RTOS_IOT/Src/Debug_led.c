#include "debug_led.h"


#ifdef USE_CMSIS_FREERTOS

	#include "cmsis_os.h"
	osThreadId taskLED_HEARTBEAT;

#else

	typedef enum {
			StartCycle,
			Systolic,
			Systolic_rest,
			diastolic,
			diastolic_rest,
	}eCardiacCycle;

	static uint32_t NextCycleTime = 0;
	static eCardiacCycle LEDCycle = StartCycle;

#endif

#ifdef TESTING_IN_RTOS
	#include "cmsis_os.h"
	osThreadId taskLED_HEARTBEAT;
#endif


static GPIO_TypeDef* PortLED;
static uint32_t LED_PIN;



void HeartBeat_LED_Init (GPIO_TypeDef * LED_Port , GPIO_InitTypeDef * GPIO){

	PortLED = LED_Port;
	LED_PIN = GPIO->Pin;

	HAL_GPIO_Init(LED_Port, GPIO);

#ifdef USE_CMSIS_FREERTOS

	osThreadDef(HeartBeat, LED_HeartBeat, osPriorityNormal, 0 , configMINIMAL_STACK_SIZE);
	taskLED_HEARTBEAT = osThreadCreate(osThread(HeartBeat),NULL);

#endif

#ifdef TESTING_IN_RTOS
	osThreadDef(HeartBeat, LED_HeartBeat, osPriorityNormal, 0 , configMINIMAL_STACK_SIZE);
	taskLED_HEARTBEAT = osThreadCreate(osThread(HeartBeat),NULL);
#endif


}



void LED_HeartBeat (void const * argument){

#ifdef USE_CMSIS_FREERTOS
	uint32_t xLastTickWakeup = osKernelSysTick();

	for(;;)
	{
		HAL_GPIO_TogglePin(PortLED, LED_PIN);
#ifdef INCLUDE_vTaskDelayUntil
		osDelayUntil(&xLastTickWakeup,220);
#else
		osDelay(220);
#endif
		HAL_GPIO_TogglePin(PortLED, LED_PIN);
#ifdef INCLUDE_vTaskDelayUntil
		osDelayUntil(&xLastTickWakeup,320);
#else
		osDelay(320);
#endif
		HAL_GPIO_TogglePin(PortLED, LED_PIN);
#ifdef INCLUDE_vTaskDelayUntil
		osDelayUntil(&xLastTickWakeup,220);
#else
		osDelay(220);
#endif
		HAL_GPIO_TogglePin(PortLED, LED_PIN);
#ifdef INCLUDE_vTaskDelayUntil
		osDelayUntil(&xLastTickWakeup,1240);
#else
		osDelay(1240);
#endif

	}
#else
#ifdef TESTING_IN_RTOS

	for(;;){

#endif

	switch (LEDCycle){
		case StartCycle:
			NextCycleTime = HAL_GetTick() + 220;
			LEDCycle = Systolic;
			HAL_GPIO_WritePin(PortLED, LED_PIN, GPIO_PIN_SET);
			break;

		case Systolic:
			if (HAL_GetTick() >= NextCycleTime){
				NextCycleTime = HAL_GetTick() + 320;
				LEDCycle++;
				HAL_GPIO_WritePin(PortLED, LED_PIN, GPIO_PIN_RESET);
			}
			break;

		case Systolic_rest:
			if (HAL_GetTick() >= NextCycleTime){
				NextCycleTime = HAL_GetTick() + 220;
				LEDCycle++;
				HAL_GPIO_WritePin(PortLED, LED_PIN, GPIO_PIN_SET);
			}
			break;

		case diastolic:
			if (HAL_GetTick() >= NextCycleTime){
				NextCycleTime = HAL_GetTick() + 1240;
				LEDCycle++;
				HAL_GPIO_WritePin(PortLED, LED_PIN, GPIO_PIN_RESET);
			}
			break;

		case diastolic_rest:
			if (HAL_GetTick() >= NextCycleTime){
				NextCycleTime = HAL_GetTick() + 220;
				LEDCycle = Systolic ;
				HAL_GPIO_WritePin(PortLED, LED_PIN, GPIO_PIN_SET);
			}
			break;

		default:
			break;
	}

#ifdef TESTING_IN_RTOS

}

#endif

#endif
}
