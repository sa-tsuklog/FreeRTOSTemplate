/*
 * GlidePhaseHeadingHold.h
 *
 *  Created on: 2015/12/18
 *      Author: sa
 */

#ifndef GLIDEPHASEHEADINGHOLD_H_
#define GLIDEPHASEHEADINGHOLD_H_

#include "../FlightParameters.h"
#include "ControllerInterface.h"
#include "../GpsGuidance.h"

class GlidePhaseHeadingHold: public ControllerInterface{
private:
	GpsGuidance* gpsGuidance;
	FlightParameters* flightParameters;
	
	int wingLatchCount = 0;
	
	float getRadRollAttitudeCommand(float radRoll,float radHeadingCommand);
public:
	GlidePhaseHeadingHold(GpsGuidance* gpsGuidance);
	void control();
	void control(float radHeadingCommand);
	void reset();
	
};

#endif /* GLIDEPHASEHEADINGHOLD_H_ */
