/*
 * BoostPhase0Control.cpp
 *
 *  Created on: 2016/01/28
 *      Author: sa
 */

#include "BoostPhase0Control.h"
#include "math.h"
#include "MyLib/Util/Util.h"
#include "MyLib/Gains/Gains.h"
#include "App/GliderControl/GliderServoControl.h"

BoostPhase0Control::BoostPhase0Control(){
	pRollGain = Util::GetInstance()->flashData.flightParameters.boostPGain[0];
	dRollGain = Util::GetInstance()->flashData.flightParameters.boostDGain[0];
	pPitchGain = Util::GetInstance()->flashData.flightParameters.boostPGain[1];
	dPitchGain = Util::GetInstance()->flashData.flightParameters.boostDGain[1];
	pHeadingGain = Util::GetInstance()->flashData.flightParameters.boostPGain[2];
	dHeadingGain = Util::GetInstance()->flashData.flightParameters.boostDGain[2];
}
void BoostPhase0Control::control(){
	control(60/180.0*M_PI,0);
}

void BoostPhase0Control::control(float radPitchCommand, float radHeadingCommand){
	float radPitch,radRoll,radHeading;
	Quaternion attitude = Gains::GetInstance()->getAttitude();
	attitude.getRadPitchRollHeading(&radPitch,&radRoll,&radHeading);
	
	Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
	
	float headingError = radHeading-radHeadingCommand;
	if(headingError < -M_PI){
		headingError += 2*M_PI;
	}else if(headingError > M_PI){
		headingError -= 2*M_PI;
	}
	
	
	float pitchCommand = -pPitchGain * (radPitch-radPitchCommand) - dPitchGain * rpsRate.y;
	float rollCommand = -pRollGain * radRoll - dRollGain * rpsRate.x;
	float yawCommand = -pHeadingGain * (headingError) - dHeadingGain * rpsRate.z;
	
	GliderServoControl::mainWingLatch();
	GliderServoControl::setPos(pitchCommand,rollCommand,yawCommand);
	
	
//	static int decimation = 0;
//	decimation = (decimation+1)%10;
//	
//	if(decimation == 0){
//		printf("pitch:(%.3f,%.3f)\t roll:(%.3f,%.3f)\t heading:(%.3f,%.3f)\r\n",radPitch*180/M_PI,radPitchCommand*180/M_PI,radRoll*180/M_PI,0.0,radHeading*180/M_PI,radHeadingCommand*180/M_PI);
//		printf("pitch %.3f\t roll %.3f\t yaw %.3f\r\n",pitchCommand,rollCommand,yawCommand);
//	}
	
}
void BoostPhase0Control::reset(){
	
}
