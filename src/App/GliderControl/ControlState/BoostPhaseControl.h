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
public:
	BoostPhaseControl();
	void control();
	void reset();
};

#endif /* BOOSTPHASECONTROL_H_ */
