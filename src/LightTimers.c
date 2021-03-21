#include "Traffic_Tasks.h"
#include "STM_GPIO_CONFIG.h"
#include "LightTimers.h"


uint8_t lightStatus;

void GREEN_Status_Timer_Callback() {

	GPIO_ResetBits(GPIO_PORT, GREEN_LIGHT_PIN);        // turn off GREEN light
	GPIO_SetBits(GPIO_PORT, AMBER_LIGHT_PIN);         // turn on AMBER light

	printf("GREEN Light OFF, AMBER light On\n");

	lightStatus = AMBER;

	xQueueOverwrite(xLightStatusQueue, &lightStatus);

	xEventGroupSetBits(xLightStatusTrasition,	lightStatus);


}

void YELLOW_Status_Timer_Callback() {

	GPIO_ResetBits(GPIO_PORT, AMBER_LIGHT_PIN);       // turn off yellow light
	GPIO_SetBits(GPIO_PORT, RED_LIGHT_PIN);            // turn on red light

	printf("YELLOW Light OFF, RED light On\n");

	lightStatus = RED;

	xQueueOverwrite(xLightStatusQueue, &lightStatus);

	xEventGroupSetBits(xLightStatusTrasition,	lightStatus);



}

void RED_Status_Timer_Callback() {

	GPIO_ResetBits(GPIO_PORT, RED_LIGHT_PIN);       // turn off RED light
	GPIO_SetBits(GPIO_PORT, GREEN_LIGHT_PIN);            // turn on GREEN light

	printf("RED Light OFF, GREEN light On\n");

	lightStatus= GREEN;

	xQueueOverwrite(xLightStatusQueue, &lightStatus);

	xEventGroupSetBits(xLightStatusTrasition,	lightStatus);


}


