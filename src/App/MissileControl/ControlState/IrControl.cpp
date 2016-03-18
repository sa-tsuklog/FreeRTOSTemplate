/*
 * IrControl.cpp
 *
 *  Created on: 2016/03/12
 *      Author: sa
 */


#include "IrControl.h"
#include "MyLib/Seeker/Seeker.h"

IrControl::IrControl(){
	//seekerNoiseFloor = Util::GetInstance()->flashData.seekerNoiseFloor;
	
	//TODO: flashから引っ張ってくる
	seekerNoiseFloor = 200;
	pPitchGain = 1;
	pRollGain = 1;
	pHeadingGain = 1;
	dPitchGain = 1;
	dRollGain = 1;
	dHeadingGain =1;
}

void IrControl::control(){
	control(0.0);
}

void IrControl::control(float radHeadingCommand){
	float upDown,leftRight,intensity;
	Seeker::GetInstance()->getDirection(&upDown,&leftRight,&intensity);
	
	if(intensity < seekerNoiseFloor){
		pitchHeadingHold(radHeadingCommand);
	}else{
		terminal(upDown,leftRight);
	}
	
}

void IrControl::pitchHeadingHold(float radHeadingCommand){
	float pitchCommand,rollCommand,yawCommand;
	float radPitch,radRoll,radHeading;
	
	Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
	Quaternion attitude = Gains::GetInstance()->getAttitude();
	
	attitude.getRadPitchRollHeading(&radPitch,&radRoll,&radHeading);
	
//	pitchCommand = upDown    * pPitchGain  - rpsRate.y * dPitchGain;
//	rollCommand  =-radRoll   * pRollGain   - rpsRate.x * dRollGain;
//	yawCommand   =  *pHeadingGain - rpsRate.z * dHeadingGain; 
	
	MissileServoControl::setPos(pitchCommand,rollCommand,yawCommand);
}

void IrControl::terminal(float upDown,float leftRight){
	float pitchCommand,rollCommand,yawCommand;
	float radPitch,radRoll,radHeading;
	
	Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
	Quaternion attitude = Gains::GetInstance()->getAttitude();
	
	attitude.getRadPitchRollHeading(&radPitch,&radRoll,&radHeading);
	
	pitchCommand = upDown    * pPitchGain  - rpsRate.y * dPitchGain;
	rollCommand  =-radRoll   * pRollGain   - rpsRate.x * dRollGain;
	yawCommand   = leftRight *pHeadingGain - rpsRate.z * dHeadingGain; 
	
	MissileServoControl::setPos(pitchCommand,rollCommand,yawCommand);
}
