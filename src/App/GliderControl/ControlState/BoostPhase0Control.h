/*
 * BoostPhase0Control.h
 *
 *  Created on: 2016/01/28
 *      Author: sa
 */

#ifndef BOOSTPHASE0CONTROL_H_
#define BOOSTPHASE0CONTROL_H_

#include "ControllerInterface.h"

class BoostPhase0Control:public ControllerInterface{
private:
	float dRollGain;
	float pRollGain;
	float dPitchGain;
	float pPitchGain;
	float dHeadingGain;
	float pHeadingGain;
public:
	BoostPhase0Control();
	void control();
	void control(float radPitchCommand, float radHeadingCommand);
	void reset();
};

#endif /* BOOSTPHASE0CONTROL_H_ */
