/*
 * ControlElement.h
 *
 *  Created on: 2016/05/12
 *      Author: sa
 */

#ifndef CONTROLELEMENT_H_
#define CONTROLELEMENT_H_

#include "MissileServoControl.h"
#include "ControlParams.h"

class ControlElement{
private:
	float SURFACE_LIMIT;
	float HOLD_ATTITUDE_P_GAIN[3];
	float HOLD_ATTITUDE_D_GAIN[3];
	float SEEKER_P_GAIN_H;
	float SEEKER_D_GAIN_H;
	float SEEKER_P_GAIN_L;
	float SEEKER_D_GAIN_L;
	float G_COMPENSATION_GAIN;
	float MANUAL_CONTROL_D_GAIN;
	
	float limit(float value,float limit);
	float wrapAround(float value,float limit);
public:
	ControlElement();
	void controlSurfaceIdle();
	void holdAttitude(float radPitchCommand,float radHeadingCommand);
	void irTerminalGuidance(float seekerUpDown,float seekerLeftRight,float seekerUpDownDelta, float seekerLeftRightDelta,bool highGain);
	void manualControl(ControlParams* params);
	void test();
};

#endif /* CONTROLELEMENT_H_ */
