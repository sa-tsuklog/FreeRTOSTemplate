/*
 * BoostPhaseControl.h
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */

#ifndef BOOSTPHASECONTROL_H_
#define BOOSTPHASECONTROL_H_

#include "ControllerInterface.h"

class BoostPhaseControl:public ControllerInterface{
private:
	float dRollGain;
	float pRollGain;
	float dPitchGain;
	float pPitchGain;
	float dHeadingGain;
	float pHeadingGain;
public:
	BoostPhaseControl();
	void control();
	void control(float radHeadingCommand);
	void reset();
};

#endif /* BOOSTPHASECONTROL_H_ */
