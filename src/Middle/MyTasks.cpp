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
