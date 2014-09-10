/*
 * TankControl.h
 *
 *  Created on: 2014/09/07
 *      Author: sa
 */

#ifndef TANKCONTROL_H_
#define TANKCONTROL_H_

#include "Common/Quaternion.h"
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
	const int DEAD_ZONE = 16;
	const float FEED_FORWARD_COEF = 10.0;
	
	float radPitchCommand;
	float radHeadingCommand;
	xQueueHandle controlParamsQueue;
	
	Quaternion calcCameraCommand(ControlParams* params);
public:
	void TankControlTask();
	void setControlParms(ControlParams* controlParams);
	static void TankControlTaskEntry(void *pvParameters);
};


#endif /* TANKCONTROL_H_ */
