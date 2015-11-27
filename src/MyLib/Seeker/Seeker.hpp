/*
 * Seeker.hpp
 *
 *  Created on: 2014/03/08
 *      Author: sa
 */

#ifndef SEEKER_HPP_
#define SEEKER_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "Filter.hpp"

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
	Filter bandPass;
	Filter allPass;
	
	void SeekerTask();
public:
	float getIntensity();
	float getRadVertical();
	float getRadHorizontal();
	
	static void SeekerTaskEntry(void *pvParameters);
	static void initSeeker();
};

#endif /* SEEKER_HPP_ */
