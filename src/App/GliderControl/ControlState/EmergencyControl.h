/*
 * EmergencyControl.h
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */

#ifndef EMERGENCYCONTROL_H_
#define EMERGENCYCONTROL_H_

#include "ControllerInterface.h"

class EmergencyControl:public ControllerInterface{
public:
	EmergencyControl();
	void control();
	void reset();
};

#endif /* EMERGENCYCONTROL_H_ */
