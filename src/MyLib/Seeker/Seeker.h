/*
 * Seeker.h
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#ifndef SEEKER_H_
#define SEEKER_H_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "Driver/Filter.hpp"

class Seeker {
	// Singleton pattern definition
private:
	Seeker();
	Seeker(const Seeker& rhs);
	Seeker& operator=(const Seeker& rhs);
	virtual ~Seeker() {}
public:
	static Seeker* GetInstance() {
		static Seeker instance;
		return &instance;
	}

	// Class definition
private:
	static const float NORMALIZED_CENTER_FREQUENCY = 0.5/(2.5/2);
	static const float Q_FACTOR = 20;
	
	Filter bandpass[4];
	Filter allpass[4];
	volatile float filteredI[4];
	volatile float filteredQ[4];
	
	SemaphoreHandle_t dataUpdateMutex;
	
	float getIntensityOfCh(int32_t ch);
	
	void SeekerTask();
public:
	void getDirection(float* outUpDown,float* outLeftRight,float* outIntensity);
	static void SeekerTaskEntry(void *pvParameters);
	static void initSeeker();
};


#endif /* SEEKER_H_ */
