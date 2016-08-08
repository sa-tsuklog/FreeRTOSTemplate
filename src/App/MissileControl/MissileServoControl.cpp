/*
 * GliderServoControl.cpp
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */
#include <pch.h>

#include "MissileServoControl.h"
#include "Device/Servo/Servo.h"

int MissileServoControl::surfaceToServoCh(Surface surface){
	if(surface == MissileServoControl::TOP_RIGHT){
		return 0;
	}else if(surface == MissileServoControl::BOTTOM_RIGHT){
		return 1;
	}else if(surface == MissileServoControl::BOTTOM_LEFT){
		return 2;
	}else if(surface == MissileServoControl::TOP_LEFT){
		return 3;
	}else{
		return 4;
	}
}

void MissileServoControl::mainWingOpen(){
	
	
}
void MissileServoControl::mainWingLatch(){
	
}
void MissileServoControl::setPos(float pitchCommand,float rollCommand,float yawCommand){
	static float lpfTopRight 	= 0;
	static float lpfBottomRight = 0;
	static float lpfBottomLeft	= 0;
	static float lpfTopLeft 	= 0;
	
	static int decimator = 0;
	
	float topRightCommand 		= -pitchCommand + rollCommand + yawCommand;
	float bottomRightCommand 	= -pitchCommand + rollCommand - yawCommand;
	float bottomLeftCommand		= +pitchCommand + rollCommand - yawCommand;
	float topLeftCommand		= +pitchCommand + rollCommand + yawCommand;
	
	lpfTopRight 	= lpfTopRight 	* SMOOTHING_FACTOR + topRightCommand 	* (1-SMOOTHING_FACTOR);
	lpfBottomRight 	= lpfBottomRight* SMOOTHING_FACTOR + bottomRightCommand * (1-SMOOTHING_FACTOR);
	lpfBottomLeft 	= lpfBottomLeft	* SMOOTHING_FACTOR + bottomLeftCommand 	* (1-SMOOTHING_FACTOR);
	lpfTopLeft 		= lpfTopLeft	* SMOOTHING_FACTOR + topLeftCommand 	* (1-SMOOTHING_FACTOR);
	
	
//	nativeSetPos(TOP_RIGHT,		lpfTopRight);
//	nativeSetPos(BOTTOM_RIGHT,  lpfBottomRight);
//	nativeSetPos(BOTTOM_LEFT,	lpfBottomLeft);
//	nativeSetPos(TOP_LEFT,		lpfTopLeft);
	
	nativeSetPos(TOP_RIGHT,		topRightCommand);
	nativeSetPos(BOTTOM_RIGHT,  bottomRightCommand);
	nativeSetPos(BOTTOM_LEFT,	bottomLeftCommand);
	nativeSetPos(TOP_LEFT,		topLeftCommand);
	
	decimator = (decimator+1)%50;
	
	if(fp != NULL){
	//if(decimator % 5 == 3){
		fprintf(fp,"$SVCMD,%.3f,%.3f,%.3f\r\n",pitchCommand,rollCommand,yawCommand);
		//printf("%.3f,%.3f,%.3f\r\n",pitchCommand,rollCommand,yawCommand);
	//}
	}
}


void MissileServoControl::nativeSetPos(Surface surface,float pos){
	Servo::GetInstance()->setPos(surfaceToServoCh(surface),pos);
}

void MissileServoControl::setLogOut(FILE* fp){
	this->fp = fp;
}
