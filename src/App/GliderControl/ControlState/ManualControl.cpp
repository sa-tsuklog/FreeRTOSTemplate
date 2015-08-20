/*
 * ManualControl.cpp
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */
#include <pch.h>

#include "ManualControl.h"
#include "../GliderServoControl.h"

#include "MyLib/Gains/Gains.h"
#include "Common/Quaternion.h"

ManualControl::ManualControl(){
}

void ManualControl::control(){
	ControlParams params = ControlParams(0,0,0,0,0,0,0,0,0,0);
	control(&params);
}

void ManualControl::control(ControlParams* params){
	float rx,ry,rz;
	if(params->camMode & ControlParams::BIT_CAM_MODE_STABILIZE){
		Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
		rx = rpsRate.x;
		ry = rpsRate.y;
		rz = rpsRate.z;
	}else{
		rx=ry=rz=0.0;
	}
	
	float pitchCommand 	= powf((params->pitch-128)/128.0,3) - ry*dyGain;
	float rollCommand 	= powf((params->roll-128) /128.0,3) - rx*dxGain;
	float yawCommand 	= powf((params->yaw-128)  /128.0,3) - rz*dzGain;
	
	GliderServoControl::setPos(pitchCommand,rollCommand,yawCommand);
}
void ManualControl::reset(){}
