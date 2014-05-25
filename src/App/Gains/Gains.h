/*
 * Gains.h
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */

#ifndef GAINS_H_
#define GAINS_H_

#include "../Util/Quaternion.h"
#include "ImuData.h"
#include "GpsData.h"
#include "FreeRTOS.h"
#include "queue.h"

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
};



#endif


