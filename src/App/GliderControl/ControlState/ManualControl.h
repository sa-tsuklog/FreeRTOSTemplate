/*
 * ManualControl.h
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */

#ifndef MANUALCONTROL_H_
#define MANUALCONTROL_H_

#include "ControlParams.h"
#include "ControllerInterface.h"

class ManualControl: public ControllerInterface{
private:
	static const float dxGain = 0.2; 
	static const float dyGain = 0.2;
	static const float dzGain = 0.2;
public:
	ManualControl();
	void control();
	void control(ControlParams* params);
	void reset();
};

#endif /* MANUALCONTROL_H_ */
