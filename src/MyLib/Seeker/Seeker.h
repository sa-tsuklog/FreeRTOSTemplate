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

#include "QuadrantSeeker.h"

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
	static const float Q_FACTOR_SLOW = 100;
	static const float Q_FACTOR_FAST = 10;
	static const float NOISE_FLOOR_SLOW = 600;	//4sigma
	static const float NOISE_FLOOR_FAST = 2700;	//4sigma
	
	QuadrantSeeker quadrantSeekerSlow;
	QuadrantSeeker quadrantSeekerFast;
	
	void SeekerTask();
	void seekerPritRawData();
public:
	void getDirectionSlow(float* outUpDown,float* outLeftRight,float* outIntensity);
	void getDirectionFast(float* outUpDown,float* outLeftRight,float* outIntensity);
	float getNoiseFloorSlow();
	float getNoiseFloorFast();
	
	static void SeekerTaskEntry(void *pvParameters);
	static void initSeeker();
};


#endif /* SEEKER_H_ */
