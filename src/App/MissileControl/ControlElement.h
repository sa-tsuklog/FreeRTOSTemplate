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
	static const float SURFACE_LIMIT = 0.4;
	
	static const float HOLD_ATTITUDE_P_GAIN[3];
	static const float HOLD_ATTITUDE_D_GAIN[3];
	
	static const float SEEKER_P_GAIN_H = 1.0;
	static const float SEEKER_D_GAIN_H = 80.0;
	static const float SEEKER_P_GAIN_L = 0.5;
	static const float SEEKER_D_GAIN_L = 40.0;
	static const float G_COMPENSATION_GAIN  = 0.2;
	
	static const float MANUAL_CONTROL_D_GAIN = 0.2;
	
	float limit(float value,float limit);
	float wrapAround(float value,float limit);
public:
	ControlElement();
	void controlSurfaceIdle();
	void holdAttitude(float radPitchCommand,float radHeadingCommand);
	void irTerminalGuidance(float seekerUpDown,float seekerLeftRight,float seekerUpDownDelta, float seekerLeftRightDelta,bool highGain);
	void manualControl(ControlParams* params);
	
};

#endif /* CONTROLELEMENT_H_ */
