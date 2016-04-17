/*
 * IrControl.h
 *
 *  Created on: 2016/03/12
 *      Author: sa
 */

#ifndef IRCONTROL_H_
#define IRCONTROL_H_

#include "../../GliderControl/ControlState/ControllerInterface.h"

class IrControl:public ControllerInterface{
private:
	float dRollGain;
	float pRollGain;
	float dPitchGain;
	float pPitchGain;
	float dHeadingGain;
	float pHeadingGain;
	
public:
	IrControl();
	void control();
	void control(float radHeadingCommand);
	void pitchHeadingHold(float radHeadingCommand);
	void terminal(float upDown,float leftRight);
	void reset();
};

#endif /* IRCONTROL_H_ */
