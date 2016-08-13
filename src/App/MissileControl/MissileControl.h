/*
 * MissileControl.h
 *
 *  Created on: 2016/05/12
 *      Author: sa
 */

#ifndef MISSILECONTROL_H_
#define MISSILECONTROL_H_

#include "FreeRTOS.h"
#include "queue.h"

#include "ControlParams.h"

class MissileControlState{
public:
	enum State{
		LAUNCH_STANDBY,
		BOOST_PHASE1,
		TERMINAL,
		MANUAL_CONTROL,
		EMERGENCY,
	};
};

class MissilePrintMode{
public:
	enum Mode{
		NONE,
		GPAIO,
		SEEKER,
	};
};


class MissileControl {
	// Singleton pattern definition
private:
	MissileControl();
	MissileControl(const MissileControl& rhs);
	MissileControl& operator=(const MissileControl& rhs);
	virtual ~MissileControl() {}
public:	
	static MissileControl* GetInstance() {
		static MissileControl instance;
		return &instance;
	}

	// Class definition
private:
	static const float VOLTAGE_LIMIT = 7.1;
	
	static const int MS_CONTROL_INTERVAL = 20;
	static const int MPSPS_LAUNCH_ACCEL_THRETHOLD = 9.8*3.0;
	static const int MS_BOOST1_LENGTH = 100;
	
	MissileControlState::State controlState;
	MissileControlState::State controlStateReturnPoint;
	xQueueHandle controlParamsQueue;
	xQueueHandle printModeQueue;
	MissilePrintMode::Mode printMode;
	float radPitchAtLaunch;
	float radHeadingToTarget;
	
	FILE* log0;
	
	void print();
	void printGpaio(FILE* fp);
	void printSeekerLog(FILE* fp);
	void controlStateUpdate(ControlParams* params);
	void MissileControlTask();
public:
	void setPrintMode(MissilePrintMode::Mode mode);
	void setControlParams(ControlParams* controlParams);
	
	static void MissileControlTaskEntry(void *pvParameters);
	static void initMissileControl();
};

#endif /* MISSILECONTROL_H_ */
