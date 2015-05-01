/*
 * DummyGps.cpp
 *
 *  Created on: 2014/07/03
 *      Author: sa
 */

#include "FreeRTOS.h"
#include "task.h"

#include "DummyGps.h"
#include "../../Gains.h"
#include "../../GpsData.h"

#include "GeneralConfig.h"

DummyGps::DummyGps(){
	available = 1;
}

void DummyGps::dummyGpsTask(){
	vTaskDelay(MS_INITIAL_DELAY);
	TickType_t lastWakeTime = xTaskGetTickCount();
	GpsData gpsData = GpsData(0,0,0,0,0,0);
	while(1){
		if(available){
			gpsData = GpsData(0,0,0,0,0,0);
			Gains::GetInstance()->appendGpsData(&gpsData);
		}
		vTaskDelayUntil(&lastWakeTime,100);
	}
}

void DummyGps::prvDummyGpsTask(void* pvParameters){
	DummyGps::GetInstance()->dummyGpsTask();
}
