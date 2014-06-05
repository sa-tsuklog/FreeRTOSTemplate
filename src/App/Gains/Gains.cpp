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
#include "../Logger/Logger.h"
#include "../../Middle/Stdout/myFileHandle.h"

#include "../../Drivers/PeriphLib/TIM2.h"
#include "../../Middle/Gps/Gps.h"

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
	
	xQueueReceive(gpsQueue,&gpsData,portMAX_DELAY);
	xQueueReceive(gpsQueue,&gpsData,portMAX_DELAY);
	gpsVel = gpsData.mpsGpsRelativeSpeed;
	gpsPos = gpsData.mGpsRelativePos;
	
	while(!imuData.isCmpsValid){
		xQueueReceive(imuQueue,&imuData,portMAX_DELAY);
	}
	
	attitude = KalmanFilter::insToAttitude(&imuData.mpspsAcl,&imuData.uTCmps);
	
	kf = new KalmanFilter(&gpsVel,&gpsPos,&attitude);
	
	
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
			
			//printNMEA(stdout,kf,&gpsData);
			
			if(i==0){
				//printIns(stdout,kf,&imuData,&gpsData);
			}
			
			//updateStartTime = TIM2Class::GetInstance()->getUsTime();
			Logger::GetInstance()->fileSemTake();
			FILE* fp = Logger::GetInstance()->getFp();
			if(fp != NULL){
				printNMEA(fp,kf,&gpsData);
				printIns2(fp,&imuData);
				//printIns(fp,kf,&imuData,&gpsData);
				//myFsync(fp);
			}
			Logger::GetInstance()->fileSemGive();
			//updateEndTime = TIM2Class::GetInstance()->getUsTime();
			//printf("write:%d[us]\r\n",updateEndTime-updateStartTime);
			
			i=(i+1)%5;
		}
		
		velocity = kf->getMpsSpeed();
		position = kf->getMPos();
		attitude = kf->getAttitude();
		//attitude = KalmanFilter::insToAttitude(&imuData.mpspsAcl,&imuData.uTCmps);
		float radHeading;
		float radPitch;
		float radRole;
		attitude.getRadPitchRoleHeading(&radPitch,&radRole,&radHeading);
		
		if(i==0){
			//printf("%d[us],%d[us]\r\n",predictEndTime-predictStartTime,updateEndTime-updateStartTime);
			//printf("%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\r\n",gpsPos.x,gpsPos.y,gpsPos.z,gpsVel.x,gpsVel.y,gpsVel.z);
			
//			printf("%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\r\n",imuData.mpspsAcl.x,imuData.mpspsAcl.y,imuData.mpspsAcl.z,imuData.rpsRate.x,imuData.rpsRate.y,imuData.rpsRate.z,imuData.uTCmps.x,imuData.uTCmps.y,imuData.uTCmps.z);
//			printf("pos:%.3f\t%.3f\t%.3f\tspd:%.3f\t%.3f\t%.3f\tpitch:%.3f\trole:%.3f\theading:%.3f\r\n",position.x,position.y,position.z,velocity.x,velocity.y,velocity.z,radPitch*180/M_PI,radRole*180/M_PI,radHeading*180/M_PI);
//			printf("%3f,%3f,%3f,%3f\r\n",attitude.w,attitude.x,attitude.y,attitude.z);
			//attitude.print();
			//attitude.printPitchRoleHeading();
		}
		//i=(i+1)%10;
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	}	
}

void Gains::printNMEA(FILE* fp,KalmanFilter* kf,GpsData* gpsData){
	Gps* gps = Gps::GetInstance();
	Quaternion mPos = kf->getMPos(); 
	Quaternion mpsSpeed = kf->getMpsSpeed();
	
	int latitude = Gps::GetInstance()->mPosXToDegX1M_Latitude(mPos.x);
	int longitude = Gps::GetInstance()->mPosYToDegX1M_Longitude(mPos.y);
	
	fprintf(fp,"$GPINS,");//tag,
	fprintf(fp,"%02d%02d%06.3f,A,",gps->getHour(),gps->getMin(),gps->getSec());//time,status,
	fprintf(fp,"%d%07.4f,N,%d%02.4f,E,",Gps::degX1MToDeg(latitude),Gps::degX1MToMin(latitude),
			Gps::degX1MToDeg(longitude),Gps::degX1MToMin(longitude));//latitude,longitude,
	fprintf(fp,"%.2f,%.2f,",Gps::mpsToKnot(sqrtf(mpsSpeed.x*mpsSpeed.x + mpsSpeed.y*mpsSpeed.y)),
						Gps::speedToDegDirection(mpsSpeed.x,mpsSpeed.y));//speed,direction,
	fprintf(fp,"%06d,",gps->getDate());//date
	fprintf(fp,",,A*00");
	fprintf(fp,"\r\n");
}

void Gains::printIns(FILE* fp,KalmanFilter* kf,ImuData* imuData,GpsData* gpsData){
	Quaternion mPos = kf->getMPos();
	Quaternion mpsSpeed = kf->getMpsSpeed();
	Quaternion attitude = kf->getAttitude();
	Quaternion acl = imuData->mpspsAcl;
	Quaternion gyro= imuData->rpsRate;
	Quaternion cmps = imuData->uTCmps;
	
	float pitch,role,heading;
	attitude.getRadPitchRoleHeading(&pitch,&role,&heading);
	
	int min = Gps::GetInstance()->getMin();
	float sec = Gps::GetInstance()->getSec();
	
	printf("$GIFUL,");
	printf("%.1f,",60*min+sec);//time,
	printf("%.3f,%.3f,%.3f,",heading*180/M_PI,pitch*180/M_PI,role*180/M_PI);//heading,pitch,role,
	printf("%.3f,%.3f,%.3f,",acl.x,acl.y,acl.z);//acl
	printf("%.3f,%.3f,%.3f,",gyro.x,gyro.y,gyro.z);//gyro
	printf("%.3f,%.3f,%.3f,",cmps.x,cmps.y,cmps.z);//cmps
	printf("%.3f,%.3f,%.3f,",mpsSpeed.x,mpsSpeed.y,mpsSpeed.z);
	printf("%.3f,%.3f,%.3f,",mPos.x,mPos.y,mPos.z);
	printf("%.3f,%.3f,%.3f,",gpsData->mGpsRelativePos.x,gpsData->mGpsRelativePos.y,gpsData->mGpsRelativePos.z);
	printf("\r\n");
}
void Gains::printIns2(FILE* fp,ImuData* imuData){
	fprintf(fp,"$GIRAW,");
	fprintf(fp,"%.3f,%.3f,%.3f,",imuData->mpspsAcl.x,imuData->mpspsAcl.y,imuData->mpspsAcl.z);
	fprintf(fp,"%.3f,%.3f,%.3f,",imuData->rpsRate.x,imuData->rpsRate.y,imuData->rpsRate.z);
	fprintf(fp,"%.3f,%.3f,%.3f,",imuData->uTCmps.x,imuData->uTCmps.y,imuData->uTCmps.z);
	fprintf(fp,"\r\n");
}

void Gains::appendInsData(ImuData* imuData){
	portBASE_TYPE result = xQueueSend(imuQueue,imuData,0);
	//printf("%f %f %f\r\n",imuData->uTCmps.x,imuData->uTCmps.y,imuData->uTCmps.z);
//	if(result != pdTRUE){
//		printf("imu queue overflow.\r\n");
//	}
}
void Gains::appendGpsData(GpsData* gpsData){
	xQueueSend(gpsQueue,gpsData,0);
}
void Gains::prvGainsTask(void *pvParameters){
	Gains::GetInstance()->gainsTask(pvParameters);
}

void Gains::resetImu(){
	//TODO: ‚±‚±”r‘¼§Œä‚·‚éB
	Gps::GetInstance()->resetRefPosition();
	kf->reset();
}
