/*
 * GliderServoControl.cpp
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */
#include <pch.h>

#include "GliderServoControl.h"
#include "MyLib/Servo/Servo.h"

FILE* GliderServoControl::fp = NULL;

int GliderServoControl::surfaceToServoCh(Surface surface){
	if(surface == GliderServoControl::TOP_RIGHT){
		return 0;
	}else if(surface == GliderServoControl::BOTTOM_RIGHT){
		return 1;
	}else if(surface == GliderServoControl::BOTTOM_LEFT){
		return 2;
	}else if(surface == GliderServoControl::TOP_LEFT){
		return 3;
	}else{
		return 4;
	}
}

void GliderServoControl::mainWingOpen(){
	
}
void GliderServoControl::mainWingLatch(){
	
}
void GliderServoControl::setPos(float pitchCommand,float rollCommand,float yawCommand){
	if(fp == NULL){
		fp = fopen("/log2","w");
	}
	static int decimator = 0;
	
	nativeSetPos(TOP_RIGHT,		-pitchCommand + rollCommand + yawCommand);
	nativeSetPos(BOTTOM_RIGHT,  -pitchCommand + rollCommand - yawCommand);
	nativeSetPos(BOTTOM_LEFT,	+pitchCommand + rollCommand - yawCommand);
	nativeSetPos(TOP_LEFT,		+pitchCommand + rollCommand + yawCommand);
	
	
	decimator = (decimator+1)%50;
	if(decimator % 5 == 3){
		fprintf(fp,"$SVCMD,%.3f,%.3f,%.3f\r\n",pitchCommand,rollCommand,yawCommand);
	}
}


void GliderServoControl::nativeSetPos(Surface surface,float pos){
	Servo::GetInstance()->setPos(surfaceToServoCh(surface),pos);
}
