

#include "STM_GPIO_CONFIG.h"
#include "LightTimers.h"
#include "Traffic_Tasks.h"



xQueueHandle xFlowQueue ;
xQueueHandle xLightStatusQueue;
xQueueHandle xTrafficQueue;



void GPIO_Hardware_Init(void);
void GPIO_ADC_Init(void);
void ADCTestTask(void);
void SW_Timer_Init(void);


void SW_Timer_Init(void) {

	xGreenStatusTimer = xTimerCreate("GreenStatusTimer",1000 / portTICK_PERIOD_MS, pdFALSE, (void *) 0,GREEN_Status_Timer_Callback);
	xYellowStatusTimer = xTimerCreate("YELLOWStatusTimer", 1000 / portTICK_PERIOD_MS, pdFALSE, (void *) 0,YELLOW_Status_Timer_Callback);
	xRedStatusTimer = xTimerCreate("REDStatusTimer", 1000 / portTICK_PERIOD_MS,pdFALSE, (void *) 0, RED_Status_Timer_Callback);

}


void Task_Init(void) {

	uint8_t initLightStatus = GREEN;

	// Initalize LED Sequence
	GPIO_ResetBits(GPIO_PORT, RED_LIGHT_PIN);       // turn off RED light
	GPIO_SetBits(GPIO_PORT, GREEN_LIGHT_PIN);            // turn on GREEN light

	uint8_t initFlowRate = 0;

	uint8_t initTrafficQueue[ROAD_LENGTH] = {0};

	// Create queue for light status

	xLightStatusQueue = xQueueCreate(StdQueueLength, StdQueueSize);
	xQueueSend(xLightStatusQueue, &initLightStatus, 0);
	vQueueAddToRegistry(xLightStatusQueue, "LightStatus_Q");

	// Create queue for Flow Rate
	xFlowQueue = xQueueCreate(StdQueueLength, StdQueueSize);
	xQueueSend(xFlowQueue, &initFlowRate, 0);
	vQueueAddToRegistry(xFlowQueue, "FlowRate_Q");


	// Create queue for Traffic Queue
	xTrafficQueue = xQueueCreate(1,sizeof(initTrafficQueue));
	xQueueSend(xTrafficQueue, &initTrafficQueue,0);
	vQueueAddToRegistry(xTrafficQueue, "Traffic_Q");




	xLightStatusTrasition = xEventGroupCreate();

	xEventGroupSetBits(xLightStatusTrasition, initLightStatus);


	xTaskCreate(Traffic_Flow_Task, "TrafficFlow", configMINIMAL_STACK_SIZE,
			NULL, PRIORITY_LEVEL1, NULL);
	xTaskCreate(Traffic_Generator_Task, "TrafficGen", configMINIMAL_STACK_SIZE,
			NULL, PRIORITY_LEVEL3, NULL);
	xTaskCreate(Sys_Display_Task, "TrafficDispaly", configMINIMAL_STACK_SIZE,
			NULL, PRIORITY_LEVEL2, NULL);
	xTaskCreate(Traffic_Light_State_Task, "LightSate", configMINIMAL_STACK_SIZE,
			NULL, PRIORITY_LEVEL4, NULL);

}




void GPIO_Hardware_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);



	GPIO_InitTypeDef 	  GPIO_InitStructLED;
	GPIO_InitTypeDef 	  GPIO_InitStructShift;



	GPIO_InitStructLED.GPIO_Pin = AMBER_LIGHT_PIN | GREEN_LIGHT_PIN | RED_LIGHT_PIN;
	GPIO_InitStructLED.GPIO_Mode 	= GPIO_Mode_OUT;
	GPIO_InitStructLED.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructLED.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
	GPIO_InitStructLED.GPIO_Speed    = GPIO_Speed_25MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructLED);

	GPIO_InitStructShift.GPIO_Pin 	= SHIFT_REG_RESET | SHIFT_REG_CLK | SHIFT_REG_DATA;
	GPIO_InitStructShift.GPIO_Mode 	= GPIO_Mode_OUT;
	GPIO_InitStructShift.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructShift.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructShift);


}



void Display_Reset_Bits_Init(void){



	// Reset All Bits on Display upon startup
	GPIO_ResetBits(GPIO_PORT, GREEN_LIGHT_PIN);
	GPIO_ResetBits(GPIO_PORT, AMBER_LIGHT_PIN);
	GPIO_ResetBits(GPIO_PORT, RED_LIGHT_PIN);


	// Reset SHIFT Reg bits up[on startup
	GPIO_ResetBits(GPIO_PORT, SHIFT_REG_RESET);
	GPIO_SetBits(GPIO_PORT, SHIFT_REG_RESET);

}



void GPIO_ADC_Init(void)
{

	// Enable ADC Interface and PORT C
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,  ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);


	ADC_InitTypeDef 	  ADC_InitStruct;
	GPIO_InitTypeDef      GPIO_InitStructADC;


	GPIO_InitStructADC.GPIO_Pin 	= ADC_INPUT_PIN;
	GPIO_InitStructADC.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
	GPIO_InitStructADC.GPIO_Mode 	= GPIO_Mode_AN;
	GPIO_InitStructADC.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructADC.GPIO_Speed 	= GPIO_Speed_25MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructADC);


	ADC_InitStruct.ADC_NbrOfConversion 			= 1;
	ADC_InitStruct.ADC_ContinuousConvMode 		= DISABLE;
	ADC_InitStruct.ADC_ScanConvMode 			= DISABLE;
	ADC_InitStruct.ADC_ExternalTrigConv 		= DISABLE;
	ADC_InitStruct.ADC_ExternalTrigConvEdge 	= ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_Resolution 				= ADC_Resolution_12b;
	ADC_InitStruct.ADC_DataAlign 				= ADC_DataAlign_Right;
	ADC_Init(ADC1, &ADC_InitStruct);

	ADC_Cmd(ADC1, ENABLE);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_84Cycles);



}



int main(void)
{

	printf("Beginning Program!! \n");

	GPIO_Hardware_Init();

	Display_Reset_Bits_Init();

	GPIO_ADC_Init();

	Task_Init();

	SW_Timer_Init();


	printf("Starting Scheduler...\n");

	vTaskStartScheduler();


	printf("Program Completed\n");




	return 0;
}




/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}

