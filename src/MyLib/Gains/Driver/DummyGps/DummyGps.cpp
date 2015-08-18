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
#include "math.h"

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

int DummyGps::mPosXToDegX1M_Latitude(float mPosX){
	float degX1M_Relative = mPosX/M_EARTH_RADIUS*180/M_PI*1000000.0f;
		return (int)degX1M_Relative+degX1MLattitudeRef;
}

int DummyGps::mPosYToDegX1M_Longitude(float mPosY){
	float degX1M_Relative = mPosY/M_EARTH_RADIUS*180/M_PI*1000000.0f/cosf(degX1MLattitudeRef*0.000001/180*M_PI);
		return (int)degX1M_Relative+degX1MLongitudeRef;
}

void DummyGps::prvDummyGpsTask(void* pvParameters){
	DummyGps::GetInstance()->dummyGpsTask();
}
