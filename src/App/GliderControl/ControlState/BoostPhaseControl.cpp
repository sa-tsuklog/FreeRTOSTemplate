/*
 * BoostPhaseControl.cpp
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */

#include "BoostPhaseControl.h"
#include "Common/Quaternion.h"
#include "../GliderServoControl.h"
#include "MyLib/Gains/Gains.h"
#include "MyLib/Util/Util.h"

BoostPhaseControl::BoostPhaseControl(){
pRollGain = Util::GetInstance()->flashData.flightParameters.boostPGain[0];
	dRollGain = Util::GetInstance()->flashData.flightParameters.boostDGain[0];
	pPitchGain = Util::GetInstance()->flashData.flightParameters.boostPGain[1];
	dPitchGain = Util::GetInstance()->flashData.flightParameters.boostDGain[1];
	pHeadingGain = Util::GetInstance()->flashData.flightParameters.boostPGain[2];
	dHeadingGain = Util::GetInstance()->flashData.flightParameters.boostDGain[2];
}

void BoostPhaseControl::control(){
	control(0.0);
}

void BoostPhaseControl::control(float radHeadingCommand){
	float radPitch,radRoll,radHeading;
	Quaternion attitude = Gains::GetInstance()->getAttitude();
	attitude.getRadPitchRollHeading(&radPitch,&radRoll,&radHeading);
	
	Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
	
	float pitchCommand = 0.0;
	float rollCommand = -pRollGain * radRoll - dRollGain * rpsRate.x;
	float yawCommand = -pHeadingGain * (radHeading-radHeadingCommand) - dHeadingGain * rpsRate.z;
	
	MissileServoControl::mainWingLatch();
	MissileServoControl::setPos(pitchCommand,rollCommand,yawCommand);
}
void BoostPhaseControl::reset(){
	
}


