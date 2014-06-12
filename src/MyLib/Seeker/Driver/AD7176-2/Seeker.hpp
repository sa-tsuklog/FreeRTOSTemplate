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
	float m_intensity;
	int m_decimate;
	xQueueHandle m_adDataQueue;
	xSemaphoreHandle m_seekerDataMutex;
	Filter m_filter;
	portTickType m_xLastWakeTime;

public:
	static const int TASK_WAIT_TIME = 1;
	portTickType DoTask();
	void EnqueAdData(float* data);
};


#endif /* SEEKER_HPP_ */
