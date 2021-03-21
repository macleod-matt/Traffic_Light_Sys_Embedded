#ifndef STM_GPIO_CONFIG_H_
#define  STM_GPIO_CONFIG_H_

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4_discovery.h"
#include <stdlib.h>
#include <stdbool.h>


/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"
#include "../FreeRTOS_Source/include/event_groups.h"


// Define portname
#define GPIO_PORT                GPIOC


// Define Colored light pins

#define GREEN_LIGHT_PIN          GPIO_Pin_2
#define AMBER_LIGHT_PIN          GPIO_Pin_1
#define RED_LIGHT_PIN            GPIO_Pin_0


// Define  Shift register pins
#define SHIFT_REG_RESET          GPIO_Pin_8
#define SHIFT_REG_CLK            GPIO_Pin_7
#define SHIFT_REG_DATA           GPIO_Pin_6





// Define POT output pin
#define ADC_INPUT_PIN  				 GPIO_Pin_3



#endif /* STMRTOSCONFIG_H_ */
