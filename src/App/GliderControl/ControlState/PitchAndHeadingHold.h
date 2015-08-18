/*
 * PitchAndHeadingHold.h
 *
 *  Created on: 2015/07/16
 *      Author: sa
 */

#ifndef PITCHANDHEADINGHOLD_H_
#define PITCHANDHEADINGHOLD_H_

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
	void control(ControlParams* params);
	void reset();
};

#endif /* PITCHANDHEADINGHOLD_H_ */
