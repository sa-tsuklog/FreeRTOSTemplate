/*
 * GliderServoControl.cpp
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */

#include "GliderServoControl.h"
#include "MyLib/Servo/Servo.h"
#include <stdio.h>

FILE* GliderServoControl::fp = NULL;

int GliderServoControl::surfaceToServoCh(Surface surface){
	if(surface == GliderServoControl::MAIN_WING){
		return 0;
	}else if(surface == GliderServoControl::RUDDER){
		return 4;
	}else if(surface == GliderServoControl::RIGHT_ELEVATOR){
		return 5;
	}else if(surface == GliderServoControl::LEFT_ELEVATOR){
		return 6;
	}else{
		return 0;
	}
}

void GliderServoControl::mainWingOpen(){
	nativeSetPos(MAIN_WING,1.0);
}
void GliderServoControl::mainWingLatch(){
	nativeSetPos(MAIN_WING,0.0);
}
void GliderServoControl::setPos(float pitchCommand,float rollCommand,float yawCommand){
	if(fp == NULL){
		fp = fopen("/log2","w");
	}
	
	
	static int decimator = 0;
	
	float rudderCommand = -yawCommand + rollCommand;
	float rightElevatorCommand = pitchCommand + rollCommand;
	float leftElevatorCommand = -pitchCommand + rollCommand;
	
	
	
	
	nativeSetPos(RUDDER,rudderCommand);
	nativeSetPos(RIGHT_ELEVATOR,rightElevatorCommand);
	nativeSetPos(LEFT_ELEVATOR,leftElevatorCommand);
	
	
	Servo::GetInstance()->setPos(0,-pitchCommand+rollCommand);
	Servo::GetInstance()->setPos(1, pitchCommand+rollCommand);
	
	decimator = (decimator+1)%50;
	if(decimator % 5 == 3){
		fprintf(fp,"$SVCMD,%.3f,%.3f,%.3f\r\n",pitchCommand,rollCommand,yawCommand);
	}
}


void GliderServoControl::nativeSetPos(Surface surface,float pos){
	Servo::GetInstance()->setPos(surfaceToServoCh(surface),pos);
}
