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

BoostPhaseControl::BoostPhaseControl(){
	pRollGain = 1.0;
	dRollGain = 1.0;
}
void BoostPhaseControl::control(){
	float radPitch,radRoll,radHeading;
	Quaternion attitude = Gains::GetInstance()->getAttitude();
	attitude.getRadPitchRollHeading(&radPitch,&radRoll,&radHeading);
	
	Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
	
	float pitchCommand = 0.0;
	float rollCommand = -pRollGain * radRoll - dRollGain * rpsRate.x;
	float yawCommand = 0.0;
	
	GliderServoControl::mainWingLatch();
	GliderServoControl::setPos(pitchCommand,rollCommand,yawCommand);
}
void BoostPhaseControl::reset(){
	
}


