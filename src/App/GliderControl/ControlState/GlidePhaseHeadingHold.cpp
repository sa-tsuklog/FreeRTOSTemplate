/*
 * GlidePhaseHeadingHold.cpp
 *
 *  Created on: 2015/12/18
 *      Author: sa
 */


/*
 * GlidePhaseHeadingHold.cpp
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */
#include <pch.h>
#include "GlidePhaseHeadingHold.h"
#include "Common/Math/Quaternion.h"
#include "../GliderServoControl.h"
#include "Device/Gains/Gains.h"
#include "Device/Util/Util.h"
#include "../FlightParameters.h"


GlidePhaseHeadingHold::GlidePhaseHeadingHold(GpsGuidance* gpsGuidance){
	this->gpsGuidance = gpsGuidance;
	flightParameters = &(Util::GetInstance()->flashData.flightParameters);
}

void GlidePhaseHeadingHold::control(){
	control(0);
}

void GlidePhaseHeadingHold::control(float radHeadingCommand){
	Quaternion attitude = Gains::GetInstance()->getAttitude();
	Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
	
	float radPitch,radRoll,radHeading;
	attitude.getRadPitchRollHeading(&radPitch,&radRoll,&radHeading);
	
	float radPitchCommand,radRollCommand,radYawCommand;
	
	float radRollAttitudeCommand = getRadRollAttitudeCommand(radHeading,radHeadingCommand);
	float radPitchAttitudeCommand = flightParameters->degGlideAngle*M_PI/180;
	
	radRollCommand  = flightParameters->glidePGain[0] * (radRollAttitudeCommand - radRoll)  - flightParameters->glideDGain[0] * rpsRate.x;
	radPitchCommand = flightParameters->glidePGain[1] * (radPitchAttitudeCommand- radPitch) - flightParameters->glideDGain[1] * rpsRate.y; 
	radYawCommand = -flightParameters->glideDGain[2] * rpsRate.z;
	
	if(wingLatchCount < 10){
		GliderServoControl::mainWingOpen();
		wingLatchCount++;
	}else{
		GliderServoControl::mainWingLatch();
	}
	GliderServoControl::setPos(radPitchCommand,radRollCommand,radYawCommand);
	
	
//	float radHeadingError = radHeadingCommand - radHeading;
//		
//	if(radHeadingError < -M_PI){
//		radHeadingError += 2*M_PI;
//	}else if(radHeadingError > M_PI){
//		radHeadingError -= 2*M_PI;
//	}
//	
//	float radRollError = radRollCommand - radRoll;
//	if(radRollError < -M_PI){
//		radRollError += 2*M_PI;
//	}else if(radRollError > M_PI){
//		radRollError -= 2*M_PI;
//	}
//  printf("heading commnad:%.1f, heading error:%.1f, roll command %.1f, roll error %.1f\r\n",radHeadingCommand*180/M_PI,radHeadingError*180/M_PI,radRollCommand*180/M_PI,radRollError*180/M_PI);
}
void GlidePhaseHeadingHold::reset(){
	wingLatchCount = 0;
}

float GlidePhaseHeadingHold::getRadRollAttitudeCommand(float radHeading,float radHeadingCommand){
	float radHeadingError = radHeadingCommand - radHeading;
	
	if(radHeadingError < -M_PI){
		radHeadingError += 2*M_PI;
	}else if(radHeadingError > M_PI){
		radHeadingError -= 2*M_PI;
	}
	
	float radRollCommand = radHeadingError * flightParameters->degPerDegHeadingToRollGain;
	if(radRollCommand < -flightParameters->degRollCommandLimit * M_PI/180){
		radRollCommand = -flightParameters->degRollCommandLimit * M_PI/180;
	}else if(radRollCommand > flightParameters->degRollCommandLimit * M_PI/180){
		radRollCommand = flightParameters->degRollCommandLimit * M_PI/180;
	}
	
	return radRollCommand;
}


