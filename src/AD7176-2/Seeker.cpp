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

float* intensity;

void prvSeekerTask(void *pvParameters){
	adDataQueue = xQueueCreate(256,sizeof(AdData));
	seekerDataMutex = xSemaphoreCreateMutex();
	
	if(adDataQueue == NULL || seekerDataMutex == NULL){
		
	}
	
	
	Filter* filter[4];
	for(int i=0;i<4;i++){
		filter[i] = new Filter(1.591/(10/4.0),100);
		
		if(filter[i] == NULL){
			printf("malloc error at prvSeekerTask\n\r");
			while(1){}
		}
		
	}
	intensity = (float*)malloc(sizeof(float)*4);
	if(intensity == NULL){
		printf("malloc error at prvSeekerTask\n\r");
		while(1){}
	}
	
	float bandpass[4];
	float allpass[4];
	AdData adData;
	
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
			for(int i=0;i<4;i++){
				bandpass[i] = filter[i]->bandpass(adData.ch[i]);
				allpass[i] = filter[i]->allpass(bandpass[i]);
			}
		}
		
		for(int i=0;i<4;i++){
			intensity[i] = filter[i]->myAbs(bandpass[i],allpass[i]);
		}
		if(decimate%100 == 0){
			printf("%f\n\r",intensity[0]);
		}
		
		decimate++;
		
		vTaskDelayUntil(&xLastWakeTime,TASK_WAIT_TIME);
	}
}

void enqueAdData(float* data){
	if(adDataQueue == NULL){
		return;
	}
	
	AdData adData;
	for(int i=0;i<4;i++){
		adData.ch[i] = data[i];
	}
	xQueueSendToBack(adDataQueue,&adData,0);
}
