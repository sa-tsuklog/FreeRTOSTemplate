/*
 * SeekerControl.h
 *
 *  Created on: 2016/02/22
 *      Author: sa
 */

#ifndef SEEKERCONTROL_H_
#define SEEKERCONTROL_H_

#include "ControlParams.h"
#include "ControllerInterface.h"
#include "../FlightParameters.h"

class PitchAndHeadingHold: public ControllerInterface{
private:
	float radPitchAttitudeCommand;
	float radRollAttitudeCommand;
	float radHeadingAttitudeCommand;
	int headingAttitudeCommandSet;
	FlightParameters* flightParameters;
public:
	PitchAndHeadingHold();
	void control();
	void reset();
};

#endif /* SEEKERCONTROL_H_ */
