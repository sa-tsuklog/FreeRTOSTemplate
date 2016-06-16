/*
 * TankControl.h
 *
 *  Created on: 2014/09/07
 *      Author: sa
 */

#ifndef TANKCONTROL_H_
#define TANKCONTROL_H_

#include "FreeRTOS.h"
#include "task.h"
#include "Common/Math/Quaternion.h"
#include "ControlParams.h"

class TankControl {
	// Singleton pattern definition
private:
	TankControl();
	TankControl(const TankControl& rhs);
	TankControl& operator=(const TankControl& rhs);
	virtual ~TankControl() {}
public:
	static TankControl* GetInstance() {
		static TankControl instance;
		return &instance;
	}

	// Class definition
private:
	TaskHandle_t tankControlHandle;
	
	static const int DEAD_ZONE = 16;
	static const float FEED_FORWARD_COEF = 10.0;
	static const float DEG_CANNON_VERTICAL_OFFSET = 12.0;
	static const float DEG_CANNON_HORIZONTAL_OFFSET = 2.7;
	
	float radPitchCommand;
	float radHeadingCommand;
	xQueueHandle controlParamsQueue;
	
	void updateCameraCommand(float degCameraV,float degCameraH);
	Quaternion calcCameraCommand(ControlParams* params);
public:
	void TankControlTask();
	void setControlParms(ControlParams* controlParams);
	static void TankControlTaskEntry(void *pvParameters);
	static void initTankControl();
};


#endif /* TANKCONTROL_H_ */
