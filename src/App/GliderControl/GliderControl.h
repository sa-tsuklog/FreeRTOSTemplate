/*
 * GliderControl.h
 *
 *  Created on: 2015/04/05
 *      Author: sa
 */

#ifndef GLIDERCONTROL_H_
#define GLIDERCONTROL_H_

#include <stdio.h>
#include "FreeRTOS.h"
#include "queue.h"

#include "ControlState/ControlStates.h"
#include "GpsGuidance.h"
#include "ControlParams.h"
#include "GliderControlTestBench.h"


class GliderPrintMode{
public:
	enum Mode{
		NONE,
		GPAIO,
	};
};

class GliderControl {
	// Singleton pattern definition
private:
	GliderControl();
	GliderControl(const GliderControl& rhs);
	GliderControl& operator=(const GliderControl& rhs);
	virtual ~GliderControl() {}
public:
	static GliderControl* GetInstance() {
		static GliderControl instance;
		return &instance;
	}

	// Class definition
private:
	static const int MS_CONTROL_INTERVAL = 20;
	static const int MS_CONTROL_WATCHDOG_MAX = 1000;
	static const int M_DISTANCE_LIMIT_FROM_COURCE = 10.0;
	
	FILE* log0;
	GpsGuidance guidance;
	ControlState::State controlState;
	ControlState::State controlStateReturnPoint;
	xQueueHandle controlParamsQueue;
	xQueueHandle printModeQueue;
	GliderControlTestBench testBench;
	int controlWatchdog;
	
	GliderPrintMode::Mode printMode;
	
	void gliderControlTask();
	void controlStateUpdate(ControlParams* params);
	void print();
	void printGpaio(FILE* fp);
public:
	GpsGuidance* getGpsGuidance();
	void setControlParams(ControlParams* controlParams);
	void setTestNumer(int testNumber);
	void setPrintMode(GliderPrintMode::Mode mode);
	static void gliderControlTaskEntry(void *pvParameters);
	static void initGliderControl();
};




#endif /* GLIDERCONTROL_H_ */
