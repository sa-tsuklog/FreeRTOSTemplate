/*
 * Seeker.cpp
 *
 *  Created on: 2014/03/08
 *      Author: sa
 */

#include "stdio.h"
#include "stdlib.h"
#include "Seeker.hpp"
#include "Filter.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "AdData.hpp"

#define TASK_WAIT_TIME 1

xQueueHandle adDataQueue;
xSemaphoreHandle seekerDataMutex;

float intensity;

void prvSeekerTask(void *pvParameters){
	adDataQueue = xQueueCreate(256,sizeof(float));
	seekerDataMutex = xSemaphoreCreateMutex();
	
	if(adDataQueue == NULL || seekerDataMutex == NULL){
		
	}
	
	
	Filter* filter;
	filter = new Filter(1.591/(15.625/2),100);
		
	if(filter == NULL){
		printf("malloc error at prvSeekerTask\n\r");
		while(1){}
	}
	
	
	float bandpass;
	float allpass;
	float adData;
	
	portTickType xLastWakeTime = xTaskGetTickCount();
	
	int j;
	float* buf1 = (float*)malloc(sizeof(float)*128);
	float* buf2 = (float*)malloc(sizeof(float)*128);
	for(int i=0;i<128;i++){
		buf1[i] = 0;
		buf2[i] = 0;
	}
	
	int decimate =0;
	while(1){
		while(xQueueReceive(adDataQueue,&adData,0) != errQUEUE_EMPTY){
			bandpass = filter->bandpass(adData);
			allpass = filter->allpass(bandpass);
			if(decimate <128){
				printf("%d %d %d\n\r",decimate,(int)adData,(int)bandpass);
			}
			decimate=(decimate+1)%8196;
		}
		
		
		intensity = filter->myAbs(bandpass,allpass);
		
		if(decimate%200 == 0){
			//printf("%d\n\r",(int)intensity[0]);
		}
		
		
		
		vTaskDelayUntil(&xLastWakeTime,TASK_WAIT_TIME);
	}
}

void enqueAdData(float data){
	if(adDataQueue == NULL){
		return;
	}
	
	xQueueSendToBack(adDataQueue,&data,0);
}
