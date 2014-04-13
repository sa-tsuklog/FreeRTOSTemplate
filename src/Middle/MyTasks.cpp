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

#include "Drivers/PeriphLib/ADC3.h"
#include "Drivers/PeriphLib/USART2.h"
#include "Drivers/PeriphLib/SPI1_TIM1.h"
#include "Drivers/PeriphLib/SPI2.h"
#include "Middle/Mpu-9250/MPU9250.h"
#include "Middle/AD7176-2/Ad7176-2Seeker.h"
#include "Middle/AD7176-2/Seeker.hpp"
#include "Middle/AD7176-2/Ad7176-2RegisterDef.h"
#include "Middle/AD7176-2/AdData.hpp"

#include "MyTasks.h"

void prvADCTask(void *pvParameters){

	ADC3Class* adc3 = ADC3Class::GetInstance();
	uint32_t ADvoltage[2];
	while (1)
	{
		ADvoltage[0] = adc3->GetData(0) * 3300 / 0xFFF;
		ADvoltage[1] = adc3->GetData(1) * 3300 / 0xFFF;

		//printf("%lx\t%lx\n\r", ADvoltage[0], ADvoltage[1]);
		//printf("%ld\t%ld\n\r", ADvoltage[0], ADvoltage[1]);

		vTaskDelay(100);
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

void prvAdis16488Task(void *pvParameters){
	while(1){
		//printf("%x,%x,%x,%x,%x,%x\n\r",spi1RxBuf[0],spi1RxBuf[1],spi1RxBuf[2],spi1RxBuf[3]
		        //,spi1RxBuf[4],spi1RxBuf[5]);
		unsigned char tmp = 0;
		unsigned char tmp2= 0;
		SPI1Class::GetInstance()->ReadWrite(&tmp,&tmp2,1);
		printf("%d\n\r",TIM_GetCounter(TIM1));

		vTaskDelay(1);
	}
}

void prvI2C2SendTask(void *pvParameters){
	MPU9250InitSend();
	while(1){
		MPU9250Send();
		vTaskDelay(50);
	}
}

void prvAd7176Task(void *pvParameters){
	void clearSemaphores();
	//initAd7176();

	unsigned char sts=0;
	float buf[4];
	if(buf == NULL){
		printf("malloc error at prvAd7176Task\n\r");
		while(1){}
	}


//	int i=0;
	while(1){
		SPI2Class::GetInstance()->WaitForDataReady();
		//buf[0] = read24(DATA);
		//sts = read8(STATUS);

//		if(i<4){
//			printf("%f %x\n\r",buf[0],sts);
//		}
//
//		i=(i+1)%400;
		if((sts&0x03) == 3){
			Seeker::GetInstance()->EnqueAdData(buf);
		}
	}
}

void prvSeekerTask(void *pvParameters){
	while(1){
		portTickType xLastWakeTime = Seeker::GetInstance()->DoTask();
		vTaskDelayUntil(&xLastWakeTime,Seeker::TASK_WAIT_TIME);
	}
}
