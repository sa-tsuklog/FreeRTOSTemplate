/*
 * ControlElement.cpp
 *
 *  Created on: 2016/05/12
 *      Author: sa
 */

#include "math.h"
#include "Common/Math/Quaternion.h"

#include "ControlElement.h"
#include "MissileServoControl.h"

#include "Device/Gains/Gains.h"
#include "MissileFlightParameters.h"
#include "Device/Util/Util.h"
#include "Device/Util/UserFlashData.h"

ControlElement::ControlElement(){
	SURFACE_LIMIT = Util::GetInstance()->flashData.missileFlightParameters.surfaceLimit;
	for(int i=0;i<3;i++){
		HOLD_ATTITUDE_P_GAIN[i] = Util::GetInstance()->flashData.missileFlightParameters.holdAttitudePGain[i];
		HOLD_ATTITUDE_D_GAIN[i] = Util::GetInstance()->flashData.missileFlightParameters.holdAttitudeDGain[i];
	}
	SEEKER_P_GAIN_H = Util::GetInstance()->flashData.missileFlightParameters.seekerPGainH;
	SEEKER_D_GAIN_H = Util::GetInstance()->flashData.missileFlightParameters.seekerDGainH;
	SEEKER_P_GAIN_L = Util::GetInstance()->flashData.missileFlightParameters.seekerPGainL;
	SEEKER_D_GAIN_L = Util::GetInstance()->flashData.missileFlightParameters.seekerDGainL;
	G_COMPENSATION_GAIN = Util::GetInstance()->flashData.missileFlightParameters.gCompensationGain;
	MANUAL_CONTROL_D_GAIN = Util::GetInstance()->flashData.missileFlightParameters.manualControlDGain;
}

float ControlElement::limit(float value,float limit){
	if(value < -limit){
		value = -limit;
	}else if(value > limit){
		value = limit;
	}
	return value;
}

float ControlElement::wrapAround(float value,float limit){
	if(value < -limit){
		value += 2*limit;
	}else if(value > limit){
		value -= 2*limit;
	}
	return value;
}

void ControlElement::controlSurfaceIdle(){
	MissileServoControl::setPos(0,0,0);
}
void ControlElement::holdAttitude(float radPitchCommand,float radHeadingCommand){
	float radPitch,radRoll,radHeading;
	Gains::GetInstance()->getAttitude().getRadPitchRollHeading(&radPitch,&radRoll,&radHeading);
	Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
	
	float radHeadingError =  wrapAround(radHeadingCommand - radHeading, M_PI);
	
	float earthFramePitchCommand = (radPitchCommand   -radPitch)  * HOLD_ATTITUDE_P_GAIN[1]; 
	float earthFrameYawCommand   = (radHeadingError)              * HOLD_ATTITUDE_P_GAIN[2]; 
			
	float pitchCommand = cosf(radRoll)*earthFramePitchCommand + sinf( radRoll)*earthFrameYawCommand       - rpsRate.y*HOLD_ATTITUDE_D_GAIN[1];
	float rollCommand  = (0                 - radRoll) *  HOLD_ATTITUDE_P_GAIN[0]						  - rpsRate.x*HOLD_ATTITUDE_D_GAIN[0];
	float yawCommand   = cosf(radRoll)*earthFrameYawCommand   + sinf(-radRoll)*earthFramePitchCommand     - rpsRate.z*HOLD_ATTITUDE_D_GAIN[2];
	
	pitchCommand = limit(pitchCommand,SURFACE_LIMIT);
	rollCommand = limit(rollCommand,SURFACE_LIMIT);
	yawCommand = limit(yawCommand,SURFACE_LIMIT);
	
	//printf("%f,\t%f,\t%f,\t%f\r\n",earthFramePitchCommand,earthFrameYawCommand,pitchCommand,yawCommand);
	
	MissileServoControl::setPos(pitchCommand,rollCommand,yawCommand);
}
void ControlElement::irTerminalGuidance(float seekerUpDown,float seekerLeftRight,float seekerUpDownDelta, float seekerLeftRightDelta,bool highGain){
	float seekerPGain;
	float seekerDGain;
	
	if(highGain){
		seekerPGain = SEEKER_P_GAIN_H;
		seekerDGain = SEEKER_D_GAIN_H;
	}else{
		seekerPGain = SEEKER_P_GAIN_H;
		seekerDGain = SEEKER_D_GAIN_L;
	}
	
	float radPitch,radRoll,radHeading;
	Gains::GetInstance()->getAttitude().getRadPitchRollHeading(&radPitch,&radRoll,&radHeading);
	Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
	
	float gCompensationMagnitude = cosf(radPitch) * cosf(radPitch) * G_COMPENSATION_GAIN;
	
	float pitchCommand = (seekerUpDown * seekerPGain) + (seekerUpDownDelta*seekerDGain)    - rpsRate.y*HOLD_ATTITUDE_D_GAIN[1] + cosf(-radRoll)*gCompensationMagnitude;
	float rollCommand  = (0                 - radRoll)   *HOLD_ATTITUDE_P_GAIN[0]          - rpsRate.x*HOLD_ATTITUDE_D_GAIN[0];
	float yawCommand   = (seekerLeftRight*seekerPGain)+ (seekerLeftRightDelta*seekerDGain) - rpsRate.z*HOLD_ATTITUDE_D_GAIN[2] + sinf(-radRoll)*gCompensationMagnitude;
	
//	float pitchCommand = (seekerUpDown * seekerPGain) + (seekerUpDownDelta*seekerDGain);
//	float rollCommand  = (0                 - radRoll)   *HOLD_ATTITUDE_P_GAIN[0];
//	float yawCommand   = (seekerLeftRight*seekerPGain)+ (seekerLeftRightDelta*seekerDGain);
	
	pitchCommand = limit(pitchCommand,SURFACE_LIMIT);
	rollCommand = limit(rollCommand,SURFACE_LIMIT);
	yawCommand = limit(yawCommand,SURFACE_LIMIT);
	
	//printf("%f,\t%f,\t%f,\t%f\r\n",seekerUpDown,seekerLeftRight,pitchCommand,yawCommand);
	
	MissileServoControl::setPos(pitchCommand,rollCommand,yawCommand);
	
}
void ControlElement::manualControl(ControlParams* params){
	float rx,ry,rz;
		if(params->camMode & ControlParams::BIT_CAM_MODE_STABILIZE){
			Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
			rx = rpsRate.x;
			ry = rpsRate.y;
			rz = rpsRate.z;
		}else{
			rx=ry=rz=0.0;
		}
		
		float pitchCommand 	= powf((params->pitch-128)/128.0,3) - ry*MANUAL_CONTROL_D_GAIN;
		float rollCommand 	= powf((params->roll-128) /128.0,3) - rx*MANUAL_CONTROL_D_GAIN;
		float yawCommand 	= powf((params->yaw-128)  /128.0,3) - rz*MANUAL_CONTROL_D_GAIN;
		
		if(params->throttle < 128){
			MissileServoControl::mainWingOpen();
		}else{
			MissileServoControl::mainWingLatch();
		}
		MissileServoControl::setPos(pitchCommand,rollCommand,yawCommand);
}

void ControlElement::test(){
	
}
