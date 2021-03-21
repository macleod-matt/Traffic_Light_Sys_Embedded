#include <LightTimers.h>
#include "STM_GPIO_CONFIG.h"
#include "Traffic_Tasks.h"

volatile uint8_t trafficFlow = 0;
volatile uint32_t illumPeriod; // Stop light illumination period

EventGroupHandle_t xLightStatusTrasition;
uint8_t carArr[ROAD_LENGTH] = { 0 }; // Initalize array of all zeros for car array


/*
 *
 * The traffic flow that enters the intersection is set by a potentiometer.
 * This task reads the value of the potentiometer at an appropriate
 * interval and sends its value to other tasks. A low potentiometer
 * resistance corresponds to light traffic and a high resistance
 * corresponds to heavy traffic
 *
 *
 *
 *
 */
void Traffic_Flow_Task(void *pvParameters) {

	printf("COMMENCING TRAFFIC FLOW TASK\n");

	uint16_t adcVal = 0;  // instantaneous adc value
	uint8_t inst_TrafficFlow = 0; // instantaneous traffic flow
	uint16_t flowTrans = 0;  // Flow Transition
	uint16_t checkFlowRate = 0;  // Check Flow Rate

	while (1) {

		ADC_SoftwareStartConv(ADC1);		    // Start conversion of ADC value
		while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));	// Wait for ADC to finish conversion
		adcVal = ADC_GetConversionValue(ADC1);       // Grab ADC value

		inst_TrafficFlow = (uint8_t) floor(adcVal / 512); // Divide isntantaneous adc Val with mod param to get distinct flow regions

		flowTrans = abs(inst_TrafficFlow - trafficFlow); // Get traffic flow transition

		trafficFlow = inst_TrafficFlow;


		if (xQueuePeek(xFlowQueue, &checkFlowRate, 500) != NULL) { // Check if a value exists in the queue

			xQueueOverwrite(xFlowQueue, &trafficFlow); // override current value
			if (flowTrans != 0) {

				printf("Flow Transition = %u, Traffic Flow = %u \n ", flowTrans,
						trafficFlow);

			}

		} else {

			xQueueSend(xFlowQueue, &trafficFlow, 1000); // Send new value to queue if not

		}

		vTaskDelay(200);

	}

	printf("TRAFFIC FLOW TASK COMPLETED.. \n");

}

/*
 *
 * This task randomly generates new traffic with a rate that is proportional
 * to the potentiometer’s value; this value is received from the Traffic
 * Flow Adjustment Task. The generated traffic is then sent to another
 * task so that it can be displayed on the road. Note that traffic can be
 * represented in many ways (e.g., a single integer, an array, or a struct).
 *
 *
 *
 */
void Traffic_Generator_Task(void *pvParameters) {

	printf("COMMENCING TRAFFIC GENERATOR TASK \n");
	uint8_t flowData;
	uint8_t newCarFlag;


	uint8_t lightStatus;


	uint8_t flowSpacing = 0;

	uint8_t temp;


	while (1) {

		xQueuePeek(xFlowQueue, &flowData, pdMS_TO_TICKS(100));
		xQueuePeek(xLightStatusQueue, &lightStatus, pdMS_TO_TICKS(100));
		printf("Light Satus = %u\n", lightStatus);
		printf(" Flow Data  :  %u \n ", flowData);

		newCarFlag = (MAX_FLOW_RATE - flowData) < (rand() % MAX_FLOW_RATE);

		printf("Size of CAR ARR = %u \n", sizeof(carArr));


		printf(" NewCarFlag :  %u \n ", newCarFlag);

		carArr[ROAD_LENGTH] = newCarFlag;

		if (!(newCarFlag) && (flowSpacing % CAR_SPACING_MIN_ADC == 0)) {

			carArr[ROAD_LENGTH ] = 1;

		}

		if (lightStatus == GREEN) {

			for (int8_t i = 0; i <= sizeof(carArr); i++) {

				temp = carArr[i + 1];

				carArr[i + 1] = carArr[i];

				carArr[i] = temp;

			}



		}

		else {
			carArr[0] = 0;

			// For loop to move up cars in array
			for (int16_t y = 0; y < ROAD_LENGTH ; y++) {


				if (y >= 12){

					if ((carArr[y] != 1) && (carArr[y + 1] == 1)) { //

						carArr[y] = carArr[y + 1]; // Move all cars in pre light queue up 1 space every cycle
						carArr[y + 1] = 0;   // Replace previous car with zero
						}

				 	}
					if (y <= 10){


						temp = carArr[y + 1];

						carArr[y + 1] = carArr[y];

						carArr[y] = temp;


				}


		}

		}



		flowSpacing ++;
		xQueueOverwrite(xTrafficQueue, &carArr); // override current carArr
		vTaskDelay(pdMS_TO_TICKS(TRAFFIC_SPEED_CONST));


	}

	printf("TRAFFIC GENERATOR TASK COMPLETED.. \n");

}

/*
 * This task controls the timing of the traffic lights and outputs its
 * current state (i.e. green state, yellow state, or red state).
 * The timing of the lights is affected by the load of the traffic which is
 * received from the Traffic Flow Adjustment Task. An increase in traffic
 * should increase the duration of the green light and decrease the
 * duration of the red light.
 *
 *
 *
 */

void Traffic_Light_State_Task(void *pvParameters) {

	printf("COMMENCING LIGHT STATE TASK\n");

	EventBits_t eventBits;

	uint8_t trafficFlowData;


	while (1) {

		eventBits = xEventGroupWaitBits(xLightStatusTrasition,
				GREEN | AMBER | RED, pdTRUE, pdFALSE, EVENT_RESPONSE_DURATION);

		xQueuePeek(xFlowQueue, &trafficFlowData, pdMS_TO_TICKS(1000));

		printf("Flow Rate = %u \n", trafficFlowData);

		// Event of  Green light
		if (eventBits & GREEN) {

			printf("LIGHT STATUS:  GREEN \n");

			// Flow = 7 ==> 3000 + 429*7 ~ 6000ms
			// Flow = 0 ==> 3000 + 429*0 = 3000ms


			illumPeriod = pdMS_TO_TICKS(GREEN_TIME_SCALAR_MS + (TIME_SCALAR_FACTOR*trafficFlowData));

			xTimerChangePeriod(xGreenStatusTimer, illumPeriod, 0);

			xTimerStart(xGreenStatusTimer, 0);

		}

		// Event of a yellow light
		if (eventBits & AMBER) {

			printf("LIGHT STATUS:  YELLOW \n");

			illumPeriod = pdMS_TO_TICKS(YELLOW_ILLUM_PERIOD_MS);

			xTimerChangePeriod(xYellowStatusTimer, illumPeriod, 0);

			xTimerStart(xYellowStatusTimer, 0);

		}

		// Event of a Red light
		if (eventBits & RED) {

			printf("LIGHT STATUS:  RED \n");

			// Flow  = 7 ==> 6000 - 429*7 ~ 3000ms
			// Flow  = 0 ==> 6000 - 429*0 = 6000ms

			illumPeriod = pdMS_TO_TICKS(RED_TIME_SCALAR_MS  - (TIME_SCALAR_FACTOR*trafficFlowData));

			xTimerChangePeriod(xRedStatusTimer, illumPeriod, 0);

			xTimerStart(xRedStatusTimer, 0);

		}
	}

	printf("TRAFFIC LIGHT TASK COMPLETED.. \n");

}

/*
 *
 * This task controls all LEDs in the system and is responsible for
 *  visualizing all vehicle traffic and the traffic lights. It receives
 *  information from the Traffic Generator Task as well as the Traffic
 *  Light State Task and controls the system’s LEDs accordingly. This
 *  task also refreshes the car LEDs at a certain interval to emulate
 *  the flow of the traffic.
 *
 */

void Sys_Display_Task(void *pvParameters) {

	printf("COMMENCING SYSTEM DISPALY TASK \n");
//	// Reset GPIO bits
	GPIO_ResetBits(GPIO_PORT, SHIFT_REG_RESET);
	GPIO_SetBits(GPIO_PORT, SHIFT_REG_RESET);



	uint8_t traffic[ROAD_LENGTH] = {0};


	while(1){

		xQueuePeek(xTrafficQueue, &traffic, pdMS_TO_TICKS(100));


		for (int8_t i = 0; i <= ROAD_LENGTH; i++)
		{
			if (traffic[i] == 1) {

				GPIO_SetBits(GPIO_PORT, SHIFT_REG_DATA);

			} else {
				GPIO_ResetBits(GPIO_PORT, SHIFT_REG_DATA);
			}

			GPIO_SetBits(GPIO_PORT, SHIFT_REG_CLK);
			GPIO_ResetBits(GPIO_PORT, SHIFT_REG_CLK);

		}

		vTaskDelay(pdMS_TO_TICKS(TRAFFIC_SPEED_CONST));

	}

}

