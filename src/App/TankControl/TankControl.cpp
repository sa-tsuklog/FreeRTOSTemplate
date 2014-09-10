/*
 * TankControl.cpp
 *
 *  Created on: 2014/09/07
 *      Author: sa
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "Math.h"
#include "stdio.h"

#include "TankControl.h"
#include "GeneralConfig.h"
#include "ControlParams.h"

#include "MyLib/MoterDriver/MoterDriver.h"
#include "MyLib/CmdServo/CmdServo.h"
#include "MyLib/Servo/Servo.h"
#include "MyLib/Gains/Gains.h"
#include "MyLib/Util/Util.h"
#include "MyLib/Util/UserFlashData.h"
#include "Common/Quaternion.h"

TankControl::TankControl(){
}

Quaternion TankControl::calcCameraCommand(ControlParams* params){
	int paramsRz = (params->rz-128);
	int paramsYaw = (params->yaw-128);
	
	if(paramsRz > DEAD_ZONE){
		paramsRz-=DEAD_ZONE;
	}else if(paramsRz < -DEAD_ZONE){
		paramsRz+=DEAD_ZONE;
	}else{
		paramsRz = 0;
	}
	
	if(paramsYaw > DEAD_ZONE){
		paramsYaw-=DEAD_ZONE;
	}else if(paramsYaw < -DEAD_ZONE){
		paramsYaw+=DEAD_ZONE;
	}else{
		paramsYaw = 0;
	}
	
	radPitchCommand+=paramsRz/1024.0;
	radHeadingCommand+=paramsYaw/1024.0;
	
	if(radPitchCommand<-M_PI/4){
		radPitchCommand = -M_PI/4;
	}else if(radPitchCommand > M_PI/2.01){
		radPitchCommand = M_PI/2.01;
	}
	
	if(radHeadingCommand < -M_PI){
		radHeadingCommand += 2*M_PI;
	}else if(radHeadingCommand > M_PI){
		radHeadingCommand -= 2*M_PI;
	}
	
	Quaternion quatCommand = Quaternion(cos(radHeadingCommand/2),0,0,sin(radHeadingCommand/2));
	Quaternion quatCommandPitch = Quaternion(cos(radPitchCommand/2),0,sin(radPitchCommand/2),0);
	quatCommand.mul(&quatCommandPitch);
	
	return quatCommand;
}

void TankControl::TankControlTask(){
	radPitchCommand=0;
	radHeadingCommand=0;
	
	//moter driver ch0, 1 must be active
	
	controlParamsQueue = xQueueCreate(1,sizeof(ControlParams));
	ControlParams params = ControlParams(0,0,0,0,0,0,0,0,0,0);
	MoterDriver::GetInstance();
	MoterDriver::GetInstance()->setPower(0,0,1,0);
	MoterDriver::GetInstance()->setPower(1,0,1,0);
	Servo::GetInstance()->start();
	vTaskDelay(MS_INITIAL_DELAY);
	vTaskDelay(MS_INITIAL_DELAY);
	
	Servo::GetInstance()->setPos(0,1.0);
	CmdServo::GetInstance()->setSlope(1,12,0);
	CmdServo::GetInstance()->setSlope(1,12,1);
	CmdServo::GetInstance()->setPunch(1,1);
	
	CmdServo::GetInstance()->on(1);
	CmdServo::GetInstance()->on(2);
	
	
	
	float leftThrottle;
	int leftDirection;
	float rightThrottle;
	int rightDirection;
	int brake;
	
	int count =0;
	
	while(1){
		xQueueReceive(controlParamsQueue,&params,portMAX_DELAY);
		
		
		/////////////////////////////////////
		// Throttle control
		/////////////////////////////////////
		leftThrottle = (128.0 - params.pitch)/128.0 + (params.roll-128.0)/256.0;
		rightThrottle = (128.0 - params.pitch)/128.0 - (params.roll-128.0)/256.0;
		
		if(leftThrottle > 0.0){
			leftDirection = 1;
		}else{
			leftDirection = 0;
			leftThrottle = -leftThrottle;
		}
		
		if(rightThrottle > 0.0){
			rightDirection = 1;
		}else{
			rightDirection = 0;
			rightThrottle = - rightThrottle;
		}
		
		brake = 0;
//		if(leftThrottle < 0.1 && rightThrottle < 0.1){
//			brake = 1;
//			leftThrottle = 1.0;
//			rightThrottle = 1.0;
//		}else{
//			brake = 0;
//		}
		
		MoterDriver::GetInstance()->setPower(0,leftThrottle,leftDirection,brake);
		MoterDriver::GetInstance()->setPower(1,rightThrottle,rightDirection,brake);
		
		/////////////////////////////////////
		// camera control
		/////////////////////////////////////
		int cameraStablize = (params.camMode&params.BIT_CAM_MODE_STABILIZE);
		
		float degCameraV;
		float degCameraH;
		
		if(cameraStablize == 0){
			degCameraH = params.cameraH/10.0;
			degCameraV = params.cameraV/10.0;
			
			
		}else{
			float radCameraPitch,radCameraRoll,radCameraHeading;
			
			Quaternion cameraCommand = calcCameraCommand(&params);
			
			Quaternion attitude = Gains::GetInstance()->getAttitude();
			
			attitude.con()->mul(&cameraCommand);
			
			attitude.getRadPitchRollHeading(&radCameraPitch,&radCameraRoll,&radCameraHeading);
			
			degCameraV = radCameraPitch*180/M_PI;
			degCameraH = radCameraHeading*180/M_PI;
			
			Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
			float feedForwordH = rpsRate.z;
			float feedForwordV = rpsRate.y;
			
			degCameraV += feedForwordV*Util::GetInstance()->flashData.dGain[2];
			degCameraH += feedForwordH*Util::GetInstance()->flashData.dGain[1];
		
		}
		
		
		
		//limit
		if(degCameraV < -45.0){
			degCameraV = -45.0;
		}
		
		CmdServo::GetInstance()->on(1);
		CmdServo::GetInstance()->on(2);
		CmdServo::GetInstance()->setPos(1,degCameraH);
		CmdServo::GetInstance()->setPos(2,degCameraV);
		
		vTaskDelay(20);
	}
}

void TankControl::setControlParms(ControlParams* controlParams){
	if(controlParamsQueue != NULL){
		xQueueOverwrite(controlParamsQueue,controlParams);
	}
}

void TankControl::TankControlTaskEntry(void *pvParameters){
	TankControl::GetInstance()->TankControlTask();
}

