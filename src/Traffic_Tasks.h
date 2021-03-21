#ifndef TRAFFIC_TASK_H_
#define TRAFFIC_TASK_H_

#include "STM_GPIO_CONFIG.h"


void Traffic_Flow_Task(void *pvParameters);

void Traffic_Generator_Task(void *pvParameters);

void Traffic_Light_State_Task(void *pvParameters);


void Sys_Display_Task(void *pvParameters );



void Task_Init(void);



// Absolute Parameters
#define MAX_TRAFFIC_QUEUE 	    (9)
#define MAX_FLOW_RATE 			(7)
#define ROAD_LENGTH 			(19)
#define PRE_LIGHT_NUM_CARS 		(7)
#define CAR_SPACING_MIN_ADC	 	(5)
#define CAR_SPACING_MAX_ADC 	(0)
#define EVENT_RESPONSE_DURATION (6000 / portTICK_PERIOD_MS)

#define TRAFFIC_SPEED_CONST 	 (500)




// Priority Values

#define PRIORITY_LEVEL1			 		 (tskIDLE_PRIORITY + 3U)
#define PRIORITY_LEVEL2  	 	 		 (tskIDLE_PRIORITY + 2U)
#define PRIORITY_LEVEL3					 (tskIDLE_PRIORITY + 1U)
#define PRIORITY_LEVEL4         		 (tskIDLE_PRIORITY)


// Construct enum for light states

typedef enum  {

	GREEN = 0x1,
	AMBER = 0x2,
	RED = 0x4

} xlightStates ;

extern EventGroupHandle_t xLightStatusTrasition;


extern volatile uint32_t illumPeriod;


extern volatile uint8_t trafficFlow;
//extern volatile uint8_t

extern xQueueHandle xLightStatusQueue;
extern xQueueHandle xFlowQueue;
extern xQueueHandle xTrafficQueue;


// Define Standard Queue length and size used in this project
#define StdQueueLength 		(1)
#define StdQueueSize	    (sizeof(uint8_t))





#endif /* TRAFFIC_TASK_H_ */
