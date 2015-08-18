/*
 * GlidePhaseControl.cpp
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */

#include "GlidePhaseControl.h"
#include "Common/Quaternion.h"
#include "../GliderServoControl.h"
#include "MyLib/Gains/Gains.h"
#include "MyLib/Util/Util.h"
#include "../FlightParameters.h"


GlidePhaseControl::GlidePhaseControl(GpsGuidance* gpsGuidance){
	this->gpsGuidance = gpsGuidance;
	flightParameters = &(Util::GetInstance()->flashData.flightParameters);
}
void GlidePhaseControl::control(){
	Quaternion attitude = Gains::GetInstance()->getAttitude();
	Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
	
	float radPitch,radRoll,radHeading;
	attitude.getRadPitchRollHeading(&radPitch,&radRoll,&radHeading);
	
	float radPitchCommand,radRollCommand,radYawCommand;
	
	float radRollAttitudeCommand = getRadRollAttitudeCommand(radHeading);
	float radPitchAttitudeCommand = flightParameters->degGlideAngle*M_PI/180;
	
	radRollCommand  = flightParameters->glidePGain[0] * (radRollAttitudeCommand - radRoll)  - flightParameters->glideDGain[0] * rpsRate.x;
	radPitchCommand = flightParameters->glidePGain[1] * (radPitchAttitudeCommand- radPitch) - flightParameters->glideDGain[1] * rpsRate.y; 
	radYawCommand = -flightParameters->glideDGain[2] * rpsRate.z;
	
	GliderServoControl::setPos(radPitchCommand,radRollCommand,radYawCommand);
}
void GlidePhaseControl::reset(){
}

float GlidePhaseControl::getRadRollAttitudeCommand(float radHeading){
	Quaternion mRelativePosition = Gains::GetInstance()->getMRelativePos();
	float mDistanceFromCourse = gpsGuidance->mDistanceFromCurrentCourse(mRelativePosition.x,mRelativePosition.y);
	
	float radHeadingCommand = gpsGuidance->radVectorToNextWaypointFromPreviousWaypoint(mRelativePosition.x,mRelativePosition.y) - mDistanceFromCourse * (flightParameters->degPerMHeadingGain*M_PI/180);
	float radHeadingError = radHeadingCommand - radHeading;
	
	
	float radRollCommand = radHeadingError * flightParameters->degPerDegHeadingToRollGain;
	if(radRollCommand < -flightParameters->degRollCommandLimit * M_PI/180){
		radRollCommand = -flightParameters->degRollCommandLimit * M_PI/180;
	}else if(radRollCommand > flightParameters->degRollCommandLimit * M_PI/180){
		radRollCommand = flightParameters->degRollCommandLimit * M_PI/180;
	}
	
	return radRollCommand;
}
