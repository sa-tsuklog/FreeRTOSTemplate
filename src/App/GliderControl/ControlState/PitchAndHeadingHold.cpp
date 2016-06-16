/*
 * PitchAndHeadingHold.cpp
 *
 *  Created on: 2015/07/16
 *      Author: sa
 */
#include <pch.h>

#include "PitchAndHeadingHold.h"
#include "../GliderServoControl.h"

#include "Device/Util/Util.h"
#include "Device/Util/UserFlashData.h"
#include "Device/Gains/Gains.h"

#include "Common/Math/Quaternion.h"

PitchAndHeadingHold::PitchAndHeadingHold(){
	flightParameters = &(Util::GetInstance()->flashData.flightParameters);
	
	radPitchAttitudeCommand = M_PI /180.0 * flightParameters->degGlideAngle;
	radHeadingAttitudeCommand = 0.0;
	radRollAttitudeCommand = 0.0;
	headingAttitudeCommandSet=0;
	
}

void PitchAndHeadingHold::control(){
	Quaternion attitude = Gains::GetInstance()->getAttitude();
	Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
	
	float radPitch,radRoll,radHeading;
	attitude.getRadPitchRollHeading(&radPitch,&radRoll,&radHeading);
	
	if(headingAttitudeCommandSet){
		if(Gains::GetInstance()->isGpsValid()){
			radHeadingAttitudeCommand = radHeading;
			headingAttitudeCommandSet = 1;
		}
	}
	
	float radRollCommand,radPitchCommand,radYawCommand;
	
	radRollCommand  = flightParameters->glidePGain[0] * (radRollAttitudeCommand    - radRoll)    - flightParameters->glideDGain[0] * rpsRate.x;
	radPitchCommand = flightParameters->glidePGain[1] * (radPitchAttitudeCommand   - radPitch)   - flightParameters->glideDGain[1] * rpsRate.y; 
	radYawCommand   = flightParameters->glidePGain[2] * (radHeadingAttitudeCommand -radHeading)  - flightParameters->glideDGain[2] * rpsRate.z;
	
	GliderServoControl::mainWingLatch();
	GliderServoControl::setPos(radPitchCommand,radRollCommand,radYawCommand);
}

void PitchAndHeadingHold::reset(){
	headingAttitudeCommandSet = 0;
}
