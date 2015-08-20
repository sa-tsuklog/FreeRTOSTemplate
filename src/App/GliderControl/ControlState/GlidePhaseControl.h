/*
 * GlidePhaseControl.h
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */

#ifndef GLIDEPHASECONTROL_H_
#define GLIDEPHASECONTROL_H_

#include "../FlightParameters.h"
#include "ControllerInterface.h"
#include "../GpsGuidance.h"

class GlidePhaseControl: public ControllerInterface{
private:
	GpsGuidance* gpsGuidance;
	FlightParameters* flightParameters;
	
	float getRadRollAttitudeCommand(float radRoll);
public:
	GlidePhaseControl(GpsGuidance* gpsGuidance);
	void control();
	void reset();
	
};

#endif /* GLIDEPHASECONTROL_H_ */
