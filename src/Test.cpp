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


xQueueHandle controlParamsQueue=NULL;

void tank(){
	//moter driver ch0, 1 must be active
	
	controlParamsQueue = xQueueCreate(1,sizeof(ControlParams));
	ControlParams params = ControlParams(0,0,0,0,0);
	MoterDriver::GetInstance();
	Servo::GetInstance()->start();
	vTaskDelay(MS_INITIAL_DELAY);
	
	Servo::GetInstance()->setPos(0,1.0);
	
	
	float leftThrottle;
	int leftDirection;
	float rightThrottle;
	int rightDirection;
	int brake;
	
	while(1){
		xQueueReceive(controlParamsQueue,&params,portMAX_DELAY);
		
		printf("%d,%d,%d,%d,%d\r\n",params.pitch,params.role,params.yaw,params.throttle,params.flaps);
		
		leftThrottle = (128.0 - params.pitch)/128.0 + (params.role-128.0)/256.0;
		rightThrottle = (128.0 - params.pitch)/128.0 - (params.role-128.0)/256.0;
		
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
		
		if(leftThrottle < 0.1 && rightThrottle < 0.1){
			brake = 1;
			leftThrottle = 1.0;
			rightThrottle = 1.0;
		}else{
			brake = 0;
		}
		
		
		MoterDriver::GetInstance()->setPower(0,leftThrottle,leftDirection,brake);
		MoterDriver::GetInstance()->setPower(1,rightThrottle,rightDirection,brake);
		
		vTaskDelay(20);
	}
}
void glider(){
	//servo ch 0-3, 4-7 must be active
	Servo* servo = Servo::GetInstance();
	controlParamsQueue = xQueueCreate(1,sizeof(ControlParams));
	ControlParams params = ControlParams(0,0,0,0,0);
	for(int i=0;i<8;i++){
		servo->setPos(i,0.0);
	}
	servo->setPos(4,-1.0);
	servo->start();
	
	vTaskDelay(MS_INITIAL_DELAY);
	
	while(1){
		xQueueReceive(controlParamsQueue,&params,portMAX_DELAY);
		float pitch,role,yaw,throttle,flap;
		pitch = (params.pitch-128.0)/128.0;
		role  = (params.role -128.0)/128.0;
		yaw   = (params.yaw  -128.0)/128.0;
		throttle = (params.throttle-128.0)/128.0;
		
		servo->setPos(0, role);
		servo->setPos(1, role);
		servo->setPos(4,-pitch);
		servo->setPos(5, pitch);
		servo->setPos(6,-yaw);
		servo->setPos(7, throttle);
		
		vTaskDelay(20);
	}
}
void gliderAuto(){
	Servo* servo = Servo::GetInstance();
	vTaskDelay(MS_INITIAL_DELAY);
	servo->start();
	
	float pitch,role,heading;
	float roleCmd,pitchCmd,yawCmd;
	ImuData imuData=ImuData(0,0,0,0,0,0,0,0,0,0,0,0);
	
	float pGain[3];
	float dGain[3];
	
	pGain[0] = 1.0;
	pGain[1] = 1.0;
	pGain[2] = 1.0;
	dGain[0] = 0.2;
	dGain[1] = 0.2;
	dGain[2] = 0.2;
	
	int i=0;
	
	while(1){
		Quaternion attitude = Gains::GetInstance()->getAttitude();
		attitude.getRadPitchRoleHeading(&pitch,&role,&heading);
		imuData = Gains::GetInstance()->getImuData();
		
		roleCmd  = -role *pGain[0] - imuData.rpsRate.x * dGain[0];
		pitchCmd = -(pitch+3.14/24)*pGain[1] - imuData.rpsRate.y * dGain[1];
		yawCmd   =                 - imuData.rpsRate.z * dGain[2];
		
		servo->setPos(0, roleCmd);
		servo->setPos(1, roleCmd);
		servo->setPos(4,-pitchCmd);
		servo->setPos(5, pitchCmd);
		servo->setPos(6, -yawCmd);
		servo->setPos(7,0);
		
		if(i==0){
//			printf("%7.3f,%7.3f,%7.3f,\tacl:%6.3f,%6.3f,%6.3f,\trate:%6.3f,%6.3f,%6.3f,\tcmps:%6.3f,%6.3f,%6.3f\r\n",
//							pitch*180/M_PI,role*180/M_PI,heading*180/M_PI,
//							imuData.mpspsAcl.x,imuData.mpspsAcl.y,imuData.mpspsAcl.z,
//							imuData.rpsRate.x,imuData.rpsRate.y,imuData.rpsRate.z,
//							imuData.uTCmps.x,imuData.uTCmps.y,imuData.uTCmps.z);
		}
		i=(i+1)%5;
		
		vTaskDelay(20);
	}
}

void prvTestTask(void* pvParamters){
	//tank();
	//glider();
	gliderAuto();
}

void setControlParms(ControlParams* controlParams){
	if(controlParamsQueue != NULL){
		xQueueOverwrite(controlParamsQueue,controlParams);
	}
}
