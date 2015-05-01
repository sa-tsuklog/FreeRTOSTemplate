/*
 * Test.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdio.h"
#include "stm32f4xx.h"

#include "Test.h"
#include "GeneralConfig.h"
#include <math.h>
#include <string.h>
//dut
#include "MyLib/Logger/Logger.h"
#include "MyLib/Util/Util.h"

#include "MyLib/MoterDriver/MoterDriver.h"
#include "MyLib/Servo/Servo.h"

#include "MyLib/Gains/Gains.h"
#include "MyLib/Gains/ImuData.h"
#include "Common/Quaternion.h"

#include "MyLib/CmdServo/CmdServo.h"

#include "MyLib/Propo/Driver/TIM3.h"

#include "MyLib/Propo/Propo.h"

#include "MyLib/Seeker/Driver/AD7176-2/Ad7176-2Seeker.h"
#include "MyLib/SignalGenerator/Driver/DAC_TIM8.h"

xQueueHandle controlParamsQueue=NULL;

float radPitchCommand=0;
float radHeadingCommand=0;

Quaternion calcCameraCommand(ControlParams* params){
	const int DEAD_ZONE = 16;
	
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


void gliderHardKill(){
	Servo* servo = Servo::GetInstance();
	
	float pitch,roll,yaw,throttle,flap;
	pitch = 0.8;
	roll  = 0.1;
	yaw   = 0.0;
	throttle = -1.0;
	
	servo->setPos(0, roll);
	servo->setPos(1, roll);
	
	servo->setPos(2, throttle);
	
	servo->setPos(4, +pitch - yaw);
	servo->setPos(5, -pitch - yaw);
	servo->setPos(6,-yaw);
	servo->setPos(7, throttle);
	
	vTaskDelay(20);
}

void glider(){
	//servo ch 0-3, 4-7 must be active
	Servo* servo = Servo::GetInstance();
	CmdServo* cmdServo = CmdServo::GetInstance();
	controlParamsQueue = xQueueCreate(1,sizeof(ControlParams));
	ControlParams params = ControlParams(0,0,0,0,0,0,0,0,0,0);
	for(int i=0;i<8;i++){
		servo->setPos(i,0.0);
	}
	servo->setPos(2,-1.0);
	servo->setPos(7,-1.0);
	servo->start();
	
	vTaskDelay(MS_INITIAL_DELAY);
	vTaskDelay(MS_INITIAL_DELAY);
	
	cmdServo->on(1);
	
	while(1){
		
		if( xQueueReceive(controlParamsQueue,&params,200) == pdTRUE){
			float pitch,roll,yaw,throttle,flap;
			pitch = (params.pitch-128.0)/128.0;
			roll  = (params.roll -128.0)/128.0;
			yaw   = (params.yaw  -128.0)/128.0;
			throttle = (params.throttle-128.0)/128.0;
			
			servo->setPos(0, roll);
			servo->setPos(1, roll);
			
			servo->setPos(2, throttle);
			
			servo->setPos(4, +pitch);
			servo->setPos(5, roll);
			servo->setPos(6,-yaw);
			servo->setPos(7, throttle);
			
			cmdServo->setPos(1,pitch*150);
			vTaskDelay(20);
		}else{
			gliderHardKill();
		}
	}
}
void gliderAuto(){
	Servo* servo = Servo::GetInstance();
	vTaskDelay(MS_INITIAL_DELAY);
	servo->start();
	
	float pitch,roll,heading;
	float rollCmd,pitchCmd,yawCmd;
	ImuData imuData=ImuData(0,0,0,0,0,0,0,0,0,0,0,0,0);
	
	float pGain[3];
	float dGain[3];
	
	pGain[0] = 1.0;
	pGain[1] = 1.0;
	pGain[2] = 1.0;
	dGain[0] = 0.2;
	dGain[1] = 0.2;
	dGain[2] = 0.2;
	
	int i=0;
	int j=0;
	
	
	while(1){
		Quaternion attitude = Gains::GetInstance()->getAttitude();
		attitude.getRadPitchRollHeading(&pitch,&roll,&heading);
		imuData = Gains::GetInstance()->getImuData();
		
		rollCmd  = -roll *pGain[0] - imuData.rpsRate.x * dGain[0];
		pitchCmd = -(pitch+3.14/24)*pGain[1] - imuData.rpsRate.y * dGain[1];
		yawCmd   =                 - imuData.rpsRate.z * dGain[2];
		
		servo->setPos(0, rollCmd);
		servo->setPos(1, rollCmd);
		servo->setPos(4,-pitchCmd);
		servo->setPos(5, pitchCmd);
		servo->setPos(6, -yawCmd);
		servo->setPos(7,0);
		
		
		if(i==0){
			printf("%7.3f,%7.3f,%7.3f,\tacl:%6.3f,%6.3f,%6.3f,\trate:%6.3f,%6.3f,%6.3f,\tcmps:%6.3f,%6.3f,%6.3f\r\n",
							pitch*180/M_PI,roll*180/M_PI,heading*180/M_PI,
							imuData.mpspsAcl.x,imuData.mpspsAcl.y,imuData.mpspsAcl.z,
							imuData.rpsRate.x,imuData.rpsRate.y,imuData.rpsRate.z,
							imuData.uTCmps.x,imuData.uTCmps.y,imuData.uTCmps.z);
		}
		i=(i+1)%5;
		
		vTaskDelay(20);
	}
}

void test(){
	int i=0;
	while(1){
		vTaskDelay(1);
	}
}

void prvTestTask(void* pvParamters){
	test();
	//glider();
	//gliderAuto();
}

void setControlParms(ControlParams* controlParams){
	if(controlParamsQueue != NULL){
		xQueueOverwrite(controlParamsQueue,controlParams);
	}
}
