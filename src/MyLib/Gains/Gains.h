/*
 * Gains.h
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */

#ifndef GAINS_H_
#define GAINS_H_

#include "Common/Quaternion.h"
#include "ImuData.h"
#include "GpsData.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "KalmanFilter.h"
#include <stdio.h>

class Gains{
	// Singleton pattern definition
private:
	Gains();
	Gains(const Gains& rhs);
	Gains& operator=(const Gains& rhs);
	virtual ~Gains() {}
public:
	static Gains* GetInstance() {
    	static Gains instance;
    	return &instance;
	}
	
	// Class definition
	xQueueHandle imuQueue;
	xQueueHandle gpsQueue;
	
private:
	KalmanFilter* kf;
	
	void gainsTask(void *pvParameters);
public:
	void appendInsData(ImuData *imuData);
	void appendGpsData(GpsData *gpsData);
	
	float getMpspsAcl(int axis);
	float getMpsSpeed(int axis);
	float getMPos(int axis);
	float getRpsRate(int axis);
	Quaternion getAttitude();
	int getLattitudeRef();
	int getLongitudeRef();
	
	static void prvGainsTask(void *pvParameters);
	void printNMEA(FILE* fp,KalmanFilter* kf,GpsData* gpsData);
	void printIns(FILE* fp,KalmanFilter* kf,ImuData* imuData,GpsData* gpsData);
	void printIns2(FILE* fp,ImuData* imuData);
	Quaternion toEarthFrame(Quaternion vec,Quaternion attitude);
	
	void resetImu();
	
	static void initGains();
};



#endif


