#ifndef LIGHTTIMERS_H
#define LIGHTTIMERS_H

#include "STM_GPIO_CONFIG.h"


extern xTimerHandle xLightStatusTimer;



// Timer Callback Functions
void GREEN_Status_Timer_Callback();
void YELLOW_Status_Timer_Callback();
void RED_Status_Timer_Callback();



void SW_Timer_Init(void);



// Light Status Timers
xTimerHandle xGreenStatusTimer;
xTimerHandle xYellowStatusTimer;
xTimerHandle xRedStatusTimer;


#define TIME_SCALAR_FACTOR 		(429)
#define RED_TIME_SCALAR_MS 		(6000)
#define YELLOW_ILLUM_PERIOD_MS 	(1000)
#define GREEN_TIME_SCALAR_MS	(3000)



#endif /*  */
