## Introduction 

This document outlines the proposed solution for the Traffic Light system specified as Project 1 in the ECE 455 Lab manual [1]. The proposed solution has been tested for functionality and meets has proven to meet the functional requirements stated in the lab manual. This solution is an embedded software capable of simulating a real time traffic stop intersection on an STM32F0 discovery board utilizing the open source STM32F4 and FreeRTOS API’s. The final solution consisted of the following FreeRTOS objects 4 xTasks [2], 3 Software timers [2] and 3 queues [2] . 
The final solution had to be capable of generating the traffic flow based on a real time analog voltage measurement, subjugated through the change in a potentiometer connected to the STM board. The final solution was demonstrated using the following program that provided a user interface for the embedded device: 

![image](https://user-images.githubusercontent.com/61804317/175792671-7c3bcbf5-0862-4c5c-bed9-bebfd5f894c9.png)


## Design Solution 

### System Overview Diagram 

![image](https://user-images.githubusercontent.com/61804317/175792693-579a62ac-a666-4691-a10c-6347b51aba8d.png)

As shown in Figure 1 above: The system overview diagram illustrating the interactions between the 4 tasks (Traffic_Flow_Task, Traffic_Generator_Task, Traffic_Light_Task and Traffic_Display_task) and the 3 queues( xFlowQueue, xLightStatusQueue and xTrafficQueue) used to transfer data between the tasks. ![image](https://user-images.githubusercontent.com/61804317/175792697-5e8a35ff-4d4f-4cef-b618-39136bc766d9.png)

## Middleware


As shown in Figure 2 below, the GPIO ports: 

![image](https://user-images.githubusercontent.com/61804317/175792707-8cc72bf2-c6a4-4e00-b332-cc4423e9ecb0.png)

The middle wear in this project describes the software interfacing that allowed for the analog to digital (ADC) conversion reading the change in analog voltage across the potentiometer from 0-3V. This was enabled  in software using the open source API stm32f4xx_adc.c with the following parameters: 

ADC sample time:  84 cycles
Number of conversions: 1
GPIO Speed: 25MHz
Resolution: 12 Bit
Trigger Level: Rising and Falling Edge 

The ADC peripherals were enabled in the function GPIO_ADC_Init  in main. A screen shot from the source code is shown below in Figure 5 : 

![image](https://user-images.githubusercontent.com/61804317/175792728-ef36a6cd-c03f-4f1a-862f-c2e983764450.png)



Figure 6 Below shows a code snippet for the software implementation to poll for the ADC conversion values in Traffic_Flow_Task. The full source code is shown Below in appendix A under Traffic_Tasks.c: 

![image](https://user-images.githubusercontent.com/61804317/175792735-ebb6ed23-27ca-4026-9649-d50ba2217d1c.png)


As shown in the above image, after a single ADC conversion, the the adc value is stored in the variable ins_TrafficFlow where it is divided 512 and then rounded down so that it could be normalized to the 0-7 range. This allowed further simplification in developing algorithms. 

The 0-7 range was discovered through measuring the maximum and minimum ADC values during testing. 

As shown in Figure 7 below: The software GPIO implementation for the shift register and peripherals. 

![image](https://user-images.githubusercontent.com/61804317/175792741-129c128f-4813-4b5b-8abe-de996ed23d98.png)

### Application Software

![image](https://user-images.githubusercontent.com/61804317/175792746-66b580e7-f254-4e5e-8398-939e4cf9100e.png)

### Traffic Generator Algorithm 

![image](https://user-images.githubusercontent.com/61804317/175792757-1c6c2350-5a92-4a07-8561-094751a32be4.png)

### System Display Algorithm 

![image](https://user-images.githubusercontent.com/61804317/175792763-9d95684e-e40e-4452-9814-eed0ee2c7080.png)


## Limitations and Possible Improvements 



1.	Using the array data structure for generating the traffic: This data structure was originally chosen because if offers optimal control and simplicity during the debug stages as each element can be individually printed, however, the solution requires significant computation as the entire block of memory must be allocated and then shifted every iteration of the while loop. This solution was subject to the bugs stated in the section above (which required Signiant amounts of time to fix) . The array also has to be copied as it is allocated to the queue, this is another expensive operation that could be mitigated using a more light weight data structure. 

2. API Function such as  rand() and floor() function from <stdlib.h> and <stdio.h> :  The rand() function was used in the following algorithm to make the probability of a new car being generated proportional to the ADC reading: 

![image](https://user-images.githubusercontent.com/61804317/175792794-3e9a20f5-fc1f-4365-b49b-2349718a5b05.png)

The rand() function is also not truly randomized, even though a failure to produce truly randomized were never discovered during the testing of this project, this would be considered a limitation for a real world application

![image](https://user-images.githubusercontent.com/61804317/175792801-e08bee9a-8594-452e-a2f1-8cdbba6d9be2.png)


For practice, for embedded programs it is best to avoid using <stdlib.h>  and <stdio.h>  they are sizable import and there is no guarantee what other residual API peripherals you are importing that would impact your heap space. 

Below list the possible Improvements that could be made to mitigate these limitations: 

1. Replacing the array data structure with a 32 bit integer: Despite this solution not giving the user as much control over accessing each individual car (bit in this case) and having 13 bits that are essentially un used as a result of the 19 available cars in the intersection, this solution is far less expensive computationally due to bit wase operators being more optimal than array indexing

2. Replacing Rand() with a hardware enabled random: This solution is more complicated as it requries the hardware to be enabled, however, this solution will produce truly randomized numbers and does not require <stdlib.h> to be imported. 

3. Replacing the 3 Light Timers with a single Timer and changing the period with each state: For modularity originally it was though that 3 timers in LightTimer.c were required for reach light (Shown below). 

![image](https://user-images.githubusercontent.com/61804317/175792829-d2099903-1e3a-447a-abbe-d982ce2eba5c.png)


However, the same effect could be accomplished with the use of a single timer and modulating the period
after each even transition in Light_Status_Task().
20


## Summary

The final solution consisted of 4 task (for the ADC processing, traffic generation, traffic light status and
display onto the user interface), 3 queues for inter task communication (flow, light status, traffic array), 3
software timers ( one for each light) and one event (for light status transitioning). The software
infrastructure for the final solution provided a lot of flexibility and modularity as required, and with
requires minimal improvements to be fully optimized.







