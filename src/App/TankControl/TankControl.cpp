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

void TankControl::updateCameraCommand(float degCameraV,float degCameraH){
	Quaternion tankAttitude = Gains::GetInstance()->getAttitude();
	Quaternion quatCameraPitch = Quaternion(cos(degCameraV/2/180*M_PI),0,sin(degCameraV/2/180*M_PI),0);
	Quaternion quatCameraHeading   = Quaternion(cos(degCameraH/2/180*M_PI),0,0,sin(degCameraH/2/180*M_PI));
	Quaternion* bodyFrameCameraAttitude = quatCameraHeading.mul(&quatCameraPitch);
	Quaternion* earthFramecameraAttitude = tankAttitude.mul(bodyFrameCameraAttitude);
	
	float dummy;
	earthFramecameraAttitude->getRadPitchRollHeading(&radPitchCommand,&dummy,&radHeadingCommand);
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
	vQueueAddToRegistry(controlParamsQueue,"control");
	
	ControlParams params = ControlParams(0,0,0,0,0,0,0,0,0,0);
	MoterDriver::GetInstance();
	MoterDriver::GetInstance()->setPower(0,0,1,0);
	MoterDriver::GetInstance()->setPower(1,0,1,0);
	Servo::GetInstance()->start();
	vTaskDelay(MS_INITIAL_DELAY);
	vTaskDelay(MS_INITIAL_DELAY);
	
	Servo::GetInstance()->setPos(0,1.0);
	CmdServo::GetInstance()->setSlope(1,28,0);
	CmdServo::GetInstance()->setSlope(1,28,1);
	CmdServo::GetInstance()->setPunch(1,1);
	
	CmdServo::GetInstance()->setSlope(2,20,0);
	CmdServo::GetInstance()->setSlope(2,20,1);
	CmdServo::GetInstance()->setPunch(2,1);
	
	CmdServo::GetInstance()->setSlope(3,20,0);
	CmdServo::GetInstance()->setSlope(3,20,1);
	CmdServo::GetInstance()->setPunch(3,1);
	
	CmdServo::GetInstance()->on(1);
	CmdServo::GetInstance()->on(2);
	CmdServo::GetInstance()->on(3);
	
	
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
		float degCannonV;
		
		if(cameraStablize == 0){
			degCameraH = params.cameraH/10.0;
			degCameraV = params.cameraV/10.0;
			
			updateCameraCommand(degCameraV,degCameraH);
			
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
			float feedForwordV = -cosf(radCameraHeading) * rpsRate.y + sinf(radCameraHeading) * rpsRate.x;
			
			degCameraV += feedForwordV*Util::GetInstance()->flashData.dGain[2];
			degCameraH += feedForwordH*Util::GetInstance()->flashData.dGain[1];
			
		}
		
		degCannonV = degCameraV;
		
		//limit
		if(degCameraV < -45.0){
			degCameraV = -45.0;
		}
		
		if(-10.0 < degCameraH && degCameraH < 10.0){
			if(degCannonV < -12.0){
				degCannonV = -12.0;
			}
	
		}else if(-120.0 < degCameraH && degCameraH < 120.0){
			if(degCannonV < -5.0){
				degCannonV = -5.0;
			}
		}else{
			if(degCannonV < -0.0){
				degCannonV = -0.0;
			}
		}
		if(degCannonV > 20.0){
			degCannonV = 20.0;
		}
		
		
		CmdServo::GetInstance()->on(1);
		CmdServo::GetInstance()->on(2);
		CmdServo::GetInstance()->on(3);
		CmdServo::GetInstance()->setPos(1,degCameraH+DEG_CANNON_HORIZONTAL_OFFSET);
		CmdServo::GetInstance()->setPos(2,degCameraV);
		CmdServo::GetInstance()->setPos(3,degCannonV+DEG_CANNON_VERTICAL_OFFSET);
		
		if(Gains::GetInstance()->printMode == GainsPrintMode::GPAIO){
			count = (count + 1)%4;
			if(count == 0){
				Quaternion attitude = Gains::GetInstance()->getAttitude();
				float pitch,roll,heading;
				attitude.getRadPitchRollHeading(&pitch,&roll,&heading);
				//$GPAIO,Latitude,N/S,Longitude,E/W,height,HDOP,pitch,roll,yaw,SpeedX,SpeedY,SpeedZ,checksum
				printf("$GPAIO,000000.000,N,000000.000,E,0.0,1.0,%.2f,%.2f,%.2f,0.0,0.0,0.0,00\r\n",pitch*180/M_PI,roll*180/M_PI,heading*180/M_PI);
				printf("$GITNK,%.3f,00\r\n",(degCameraH+DEG_CANNON_HORIZONTAL_OFFSET));
			}
		}
		
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
void TankControl::initTankControl(){
	xTaskCreate(&TankControl::TankControlTaskEntry,"tank",2048,NULL,2,&(TankControl::GetInstance()->tankControlHandle));
}
