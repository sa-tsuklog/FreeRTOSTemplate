/*
 * Tasks.cpp
 *
 *  Created on: 2014/03/14
 *      Author: Lark_mp
 */

#include "stm32f4xx.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"

#include "PeriphLib/ADC3.h"
#include "PeriphLib/USART2.h"

#include "Tasks.h"

void ADCTask(void *pvParameters){

	ADC3Class* adc3 = ADC3Class::GetInstance();
	uint32_t ADvoltage[2];
	while (1)
	{
		ADvoltage[0] = adc3->GetData(0) * 3300 / 0xFFF;
		ADvoltage[1] = adc3->GetData(1) * 3300 / 0xFFF;

		printf("%lx\t%lx\n\r", ADvoltage[0], ADvoltage[1]);
		//printf("%ld\t%ld\n\r", ADvoltage[0], ADvoltage[1]);

		vTaskDelay(100);
		/*
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_14);
		vTaskDelay(ADvoltage[0]);
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)&(~GPIO_Pin_14));

		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_15);
		vTaskDelay(ADvoltage[0]);
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)&(~GPIO_Pin_15));
		*/
	}
}

void prvTxTask(void *pvParameters){
	portTickType xLastWakeTime = xTaskGetTickCount();

	if((USART_TypeDef*)pvParameters == USART2){
		while(1){
			USART2Class::GetInstance()->Tx();
			vTaskDelayUntil(&xLastWakeTime,1);
		}
	}
}

void prvRxTask(void *pvParameters){
	if((USART_TypeDef*)pvParameters == USART2){
		while(1){
			USART2Class::GetInstance()->Rx();
			vTaskDelay(100);

		}
	}
}

