/*
 * Gains.cpp

 *
 *  Created on: 2014/04/13
 *      Author: sa
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "../../GeneralConfig.h"
#include "Gains.h"
#include "KalmanFilter.h"
#include "stdio.h"
#include "math.h"

#include "../../Drivers/PeriphLib/TIM2.h"

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

Gains::Gains(){
	gpsQueue = xQueueCreate(1,sizeof(GpsData));
	imuQueue = xQueueCreate(2,sizeof(ImuData));
}

void Gains::gainsTask(void *pvParameters){
	int i=0;
	ImuData imuData = ImuData(0,0,0,0,0,0,0,0,0,0,0,0);
	GpsData gpsData = GpsData(0,0,0,0,0,0);
	
	int predictStartTime;
	int predictEndTime;
	int updateStartTime;
	int updateEndTime;
	
	Quaternion attitude;
	Quaternion gpsVel;
	Quaternion gpsPos;
	Quaternion velocity;
	Quaternion position;
	
	vTaskDelay(MS_INITIAL_DELAY);
	
	 
	while(!imuData.isCmpsValid){
		xQueueReceive(imuQueue,&imuData,portMAX_DELAY);
	}
	xQueueReceive(gpsQueue,&gpsData,portMAX_DELAY);
	xQueueReceive(gpsQueue,&gpsData,portMAX_DELAY);
	gpsVel = gpsData.mpsGpsRelativeSpeed;
	gpsPos = gpsData.mGpsRelativePos;
	
	attitude = KalmanFilter::intToAttitude(&imuData.mpspsAcl,&imuData.uTCmps);
	
	KalmanFilter* kf = new KalmanFilter(&gpsVel,&gpsPos,&attitude);
	
	
	while(1){
		xQueueReceive(imuQueue,&imuData,portMAX_DELAY);
		
		GPIO_SetBits(GPIOB,GPIO_Pin_8);
		predictStartTime = TIM2Class::GetInstance()->getUsTime();
		kf->predict(&imuData.mpspsAcl,&imuData.rpsRate);
		predictEndTime = TIM2Class::GetInstance()->getUsTime();

		if(xQueueReceive(gpsQueue,&gpsData,0) == pdTRUE){
			Quaternion gpsVel = gpsData.mpsGpsRelativeSpeed;
			Quaternion gpsPos = gpsData.mGpsRelativePos;
			updateStartTime = TIM2Class::GetInstance()->getUsTime();
			kf->update(&gpsVel,&gpsPos,&imuData.uTCmps);
			updateEndTime = TIM2Class::GetInstance()->getUsTime();
		}
		
		velocity = kf->getMpsSpeed();
		position = kf->getMPos();
		attitude = kf->getAttitude();
		float radHeading;
		float radPitch;
		float radRole;
		attitude.getRadPitchRoleHeading(&radPitch,&radRole,&radHeading);
		
		if(i==0){
			//printf("%d[us],%d[us]\n\r",predictEndTime-predictStartTime,updateEndTime-updateStartTime);
			//printf("%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n\r",gpsPos.x,gpsPos.y,gpsPos.z,gpsVel.x,gpsVel.y,gpsVel.z);
			
			//printf("%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n\r",imuData.mpspsAcl.x,imuData.mpspsAcl.y,imuData.mpspsAcl.z,imuData.rpsRate.x,imuData.rpsRate.y,imuData.rpsRate.z,imuData.uTCmps.x,imuData.uTCmps.y,imuData.uTCmps.z);
			printf("pos:%.3f\t%.3f\t%.3f\tspd:%.3f\t%.3f\t%.3f\tpitch:%.3f\trole:%.3f\theading:%.3f\n\r",position.x,position.y,position.z,velocity.x,velocity.y,velocity.z,radPitch*180/M_PI,radRole*180/M_PI,radHeading*180/M_PI);
			//attitude.print();
			//attitude.printPitchRoleHeading();
		}
		
		i=(i+1)%10;
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	}	
}

void Gains::appendInsData(ImuData* imuData){
	portBASE_TYPE result = xQueueSend(imuQueue,imuData,0);
	//printf("%f %f %f\n\r",imuData->uTCmps.x,imuData->uTCmps.y,imuData->uTCmps.z);
//	if(result != pdTRUE){
//		printf("imu queue overflow.\n\r");
//	}
}
void Gains::appendGpsData(GpsData* gpsData){
	xQueueSend(gpsQueue,gpsData,0);
}
void Gains::prvGainsTask(void *pvParameters){
	Gains::GetInstance()->gainsTask(pvParameters);
}
