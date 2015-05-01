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

#include "GainsConfig.h"
#include "Gains.h"
#include "KalmanFilter.h"
#include "stdio.h"
#include "math.h"

#include "Driver/Gps/Gps.h"
#include "Driver/Adis16488/Adis16488.hpp"
#include "Driver/Gps/USART2.h"
#include "Driver/Mpu9250/MPU9250.h"
#include "Driver/DummyGps/DummyGps.h"


#include "MyLib/Util/Util.h"

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"


Gains::Gains(){
	gpsQueue = xQueueCreate(1,sizeof(GpsData));
	vQueueAddToRegistry(gpsQueue,"gps");
	imuQueue = xQueueCreate(2,sizeof(ImuData));
	vQueueAddToRegistry(imuQueue,"imu");
	printModeQueue = xQueueCreate(1,sizeof(int));
	vQueueAddToRegistry(printModeQueue,"printMode");
	imuData = ImuData(0,0,0,0,0,0,0,0,0,0,0,0,0);
	gpsData = GpsData(0,0,0,0,0,0);
	attitude = Quaternion(1,0,0,0);
	mpsSpeed = Quaternion(0,0,0,0);
	mRelativePos = Quaternion(0,0,0,0);
	
	dataUpdateMutex = xSemaphoreCreateMutex();
	
	gpsWatchDog = 0;
	
	printMode = GainsPrintMode::GPAIO;
}

void Gains::prvGainsTask(void *pvParameters){
	Gains::GetInstance()->gainsTask(pvParameters);
}

void Gains::gainsTask(void *pvParameters){
	/////////////////////////////////////
	// declaration
	/////////////////////////////////////
	int predictStartTime;
	int predictEndTime;
	int updateStartTime;
	int updateEndTime;
	
	ImuData tmpImuData = ImuData(0,0,0,0,0,0,0,0,0,0,0,0,0);
	GpsData tmpGpsData = GpsData(0,0,0,0,0,0);
	
	Quaternion gpsVel;
	Quaternion gpsPos;
	
	float paRefPressure;
	float mHeight;
	float mPreviousHeight;
	
	vTaskDelay(MS_INITIAL_DELAY);
	
	/////////////////////////////////////
	//  wait for initial data
	/////////////////////////////////////
	//gps initial data
	printf("wait for gps data\r\n");
	if(xQueueReceive(gpsQueue,&tmpGpsData,MS_GPS_INITIAL_WAIT_TIME) == pdTRUE){
		gpsVel = tmpGpsData.mpsGpsSpeed;
		gpsPos = tmpGpsData.mGpsRelativePos;
	}else{
		gpsVel = Quaternion(0,0,0,0);
		gpsPos = Quaternion(0,0,0,0);
	}
	printf("gps data received\r\n");
	
	printf("wait for imu data\r\n");
	//ins initial data
	xQueueReceive(imuQueue,&tmpImuData,portMAX_DELAY);
	xQueueReceive(imuQueue,&tmpImuData,portMAX_DELAY);
	while(!tmpImuData.isCmpsValid){
		xQueueReceive(imuQueue,&tmpImuData,portMAX_DELAY);
	}
	printf("imu data received\r\n");
	
	attitude = KalmanFilter::insToAttitude(&tmpImuData.mpspsAcl,&tmpImuData.uTCmps);
	paRefPressure = tmpImuData.paPressure;
	
	//initialize kalman filter
	float secTimeStep;
	if(IMU_TYPE == ImuType::ADIS16488){
		secTimeStep = 1.0f/102.5f;
	}else if(IMU_TYPE == ImuType::MPU9250 || IMU_TYPE == ImuType::MPU9250_BMP850){
		secTimeStep = 1.0f/100.0f;
	}
	kf = new KalmanFilter(secTimeStep,&gpsVel,&gpsPos,&attitude);
	
	while(1){
		/////////////////////////////////////
		// predict
		/////////////////////////////////////
		xQueueReceive(imuQueue,&tmpImuData,portMAX_DELAY);
		xSemaphoreTake(dataUpdateMutex,portMAX_DELAY);
		imuData = tmpImuData;
		xSemaphoreGive(dataUpdateMutex);
		
		GPIO_SetBits(GPIOB,GPIO_Pin_8);
		kf->predict(&tmpImuData.mpspsAcl,&tmpImuData.rpsRate);
		
		mPreviousHeight = mHeight;
		mHeight = tmpImuData.paToRelativeHeight(paRefPressure,paRefPressure);
		
		/////////////////////////////////////
		// update
		/////////////////////////////////////
		if(xQueueReceive(gpsQueue,&tmpGpsData,0) == pdTRUE){
			xSemaphoreTake(dataUpdateMutex,portMAX_DELAY);
			gpsData = tmpGpsData;
			xSemaphoreGive(dataUpdateMutex);
			
			gpsVel = tmpGpsData.mpsGpsSpeed;
			gpsPos = tmpGpsData.mGpsRelativePos;
			
			if(IMU_TYPE == ImuType::ADIS16488 || IMU_TYPE == ImuType::MPU9250_BMP850){
				gpsVel.z = (mHeight-mPreviousHeight)/kf->secTimeStep;
				gpsPos.z = mHeight;
			}
			
			kf->update(&gpsVel,&gpsPos,&tmpImuData.uTCmps);
			
			gpsWatchDog = GPS_WATCHDOG_MAX;
		}
		
		xSemaphoreTake(dataUpdateMutex,portMAX_DELAY);
		attitude = kf->getAttitude();
		mpsSpeed = kf->getMpsSpeed();
		mRelativePos = kf->getMPos();
		xSemaphoreGive(dataUpdateMutex);
		
		if(gpsWatchDog > 0){
			gpsWatchDog--;
		}
		/////////////////////////////////////
		// print
		/////////////////////////////////////
		xQueueReceive(printModeQueue,&printMode,0);
		
		print();
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	}	
}

void Gains::print(){
	static int decimator=0;
	
	decimator = (decimator+1)%10;
	
	if(printMode == GainsPrintMode::NONE){
		//do nothing.
	}else if(printMode == GainsPrintMode::MARITIME){
		if(decimator%10 == 0){
			float radHeading;
			float radPitch;
			float radRole;
			attitude.getRadPitchRollHeading(&radPitch,&radRole,&radHeading);
			
			if(gpsWatchDog == 0){
				printf("i,?,?,");
			}else{
				int degX1MLattitude = Gps::GetInstance()->mPosXToDegX1M_Latitude(gpsData.mGpsRelativePos.x);
				int degX1MLongitude = Gps::GetInstance()->mPosYToDegX1M_Longitude(gpsData.mGpsRelativePos.y);
				
				printf("i,%d.%d,%d.%d,",degX1MLattitude/1000000,degX1MLattitude%1000000
									   ,degX1MLongitude/1000000,degX1MLongitude%1000000);
			}
			
			printf("%.3f,%.3f,%.3f,",radPitch*180/M_PI,radRole*180/M_PI,radHeading*180/M_PI);
			
			if(gpsWatchDog == 0){
				printf("?,?,");
			}else{
				printf("%.3f,%.3f,",gpsData.mpsGpsSpeed.x,gpsData.mpsGpsSpeed.y);
			}
			
			printf("%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n",
					imuData.mpspsAcl.x,imuData.mpspsAcl.y,imuData.mpspsAcl.z,
					imuData.rpsRate.x*180/M_PI,imuData.rpsRate.y*180/M_PI,imuData.rpsRate.z*180/M_PI,
					imuData.uTCmps.x,imuData.uTCmps.y,imuData.uTCmps.z);
			
		}
	}else if(printMode == GainsPrintMode::E_FRAME){
		if(decimator %10 ==0){
			Quaternion earthFrameAcl;
			Quaternion attitudeCon;
			attitudeCon.con(&attitude);
			earthFrameAcl.mul(&attitude,&(imuData.mpspsAcl));
			earthFrameAcl.mul(&attitudeCon);
			
			Quaternion earthFrameCmps;
			earthFrameCmps.mul(&attitude,&(imuData.uTCmps));
			earthFrameCmps.mul(&attitudeCon);
			
			printf("acl:%6.3f,%6.3f,%6.3f,cmps:%6.3f,%6.3f,%6.3f\r\n",earthFrameAcl.x,earthFrameAcl.y,earthFrameAcl.z,earthFrameCmps.x,earthFrameCmps.y,earthFrameCmps.z);
		
		}
	}else if(printMode == GainsPrintMode::INS){
		if(decimator % 10 == 0){
			printf("%.3f,%.3f,%.3f\t",imuData.mpspsAcl.x,imuData.mpspsAcl.y,imuData.mpspsAcl.z);
			printf("%.3f,%.3f,%.3f\t",imuData.rpsRate.x,imuData.rpsRate.y,imuData.rpsRate.z);
			printf("%.3f,%.3f,%.3f\t",imuData.uTCmps.x,imuData.uTCmps.y,imuData.uTCmps.z);
			printf("%.3f\r\n",imuData.degTemp);
		}
	}else if(printMode == GainsPrintMode::QUATERNION){
		if(decimator %2 == 0){
			printf("$GIQAT,%.5f,%.5f,%.5f,%.5f\r\n",attitude.w,attitude.x,attitude.y,attitude.z);
		}
	}else if(printMode == GainsPrintMode::GPAIO){
		if(decimator % 10 == 0){
			//$GPAIO,Latitude,N/S,Longitude,E/W,height,HDOP,pitch,roll,yaw,SpeedX,SpeedY,SpeedZ,GpsValid,checksum
			float pitch,roll,heading;
			attitude.getRadPitchRollHeading(&pitch,&roll,&heading);
			
			if(Util::GetInstance()->flashData.gpsType == GpsType::USART_GPS){
				int degX1MLattitude = Gps::GetInstance()->mPosXToDegX1M_Latitude(mRelativePos.x);
				int degX1MLongitude = Gps::GetInstance()->mPosYToDegX1M_Longitude(mRelativePos.y);				
				
				float height =  mRelativePos.z;
				int gpsValid;
				if(gpsWatchDog > GPS_WATCHDOG_MAX - 12){//10Hz = 10times of imu update + margin.
					gpsValid = 1;
				}else{
					gpsValid = 0;
				}
				
				printf("$GPAIO,%d%.6f,N,%d%.6f,E,%.2f,1,",degX1MLattitude/1000000,60.0*(degX1MLattitude%1000000)/1000000,degX1MLongitude/1000000,60.0*(degX1MLongitude%1000000)/1000000,height);//lattitude,longitude,height,HDOP
				printf("%.3f,%.3f,%.3f,",pitch*180/M_PI,roll*180/M_PI,heading*180/M_PI);//pitch,roll,heading
				printf("%.3f,%.3f,%.3f,",mpsSpeed.x,mpsSpeed.y,mpsSpeed.z);//SpeedX,Y,Z
				printf("%d,",gpsValid);//GpsValid
				printf("00\r\n");//checksum
			}else{
				printf("$GPAIO,000000.000,N,000000.000,E,0.0,1.0,%.2f,%.2f,%.2f,0.0,0.0,0.0,0,00\r\n",pitch*180/M_PI,roll*180/M_PI,heading*180/M_PI);
			}
		}
	}else{//debug
		if(decimator % 10 == 0){
			float radHeading;
			float radPitch;
			float radRole;
			attitude.getRadPitchRollHeading(&radPitch,&radRole,&radHeading);
			//Util::GetInstance()->myFprintf(0,stdout,"eacl:%.3f\t%.3f\t%.3f\tgyro:%.5f\t%.5f\t%.5f\r\n",earthFrameAcl.x,earthFrameAcl.y,earthFrameAcl.z,imuData.rpsRate.x,imuData.rpsRate.y,imuData.rpsRate.z);
			//Util::GetInstance()->myFprintf(0,stdout,"%d[us],%d[us]\r\n",predictEndTime-predictStartTime,updateEndTime-updateStartTime);
			//Util::GetInstance()->myFprintf(0,stdout,"pos %.3f\t%.3f\t%.3f\tspd: %.3f\t%.3f\t%.3f\r\n",gpsPos.x,gpsPos.y,gpsPos.z,gpsVel.x,gpsVel.y,gpsVel.z);
			printf("acl:%.3f\t%.3f\t%.3f\tgyro:%.6f\t%.6f\t%.6f\tcmps:%.3f\t%.3f\t%.3f\r\n",imuData.mpspsAcl.x,imuData.mpspsAcl.y,imuData.mpspsAcl.z,imuData.rpsRate.x,imuData.rpsRate.y,imuData.rpsRate.z,imuData.uTCmps.x,imuData.uTCmps.y,imuData.uTCmps.z);
			printf("pos:%.3f\t%.3f\t%.3f\tspd:%.3f\t%.3f\t%.3f\tpitch:%.3f\trole:%.3f\theading:%.3f\r\n",mRelativePos.x,mRelativePos.y,mRelativePos.z,mpsSpeed.x,mpsSpeed.y,mpsSpeed.z,radPitch*180/M_PI,radRole*180/M_PI,radHeading*180/M_PI);
			//printf("i,%.3f.%.3f,%.3f.%.3f,%.3f,%.3f\r\n",mRelativePos.x,mRelativePos.y,radHeading*180/M_PI,mpsSpeed.x,mpsSpeed.y,imuData.rpsRate.z);
		}
	}
}

void Gains::printNMEA(FILE* fp,KalmanFilter* kf,GpsData* gpsData){
	Gps* gps = Gps::GetInstance();
	Quaternion mPos = kf->getMPos(); 
	Quaternion mpsSpeed = kf->getMpsSpeed();
	
	int latitude = Gps::GetInstance()->mPosXToDegX1M_Latitude(mPos.x);
	int longitude = Gps::GetInstance()->mPosYToDegX1M_Longitude(mPos.y);
	
	Util::GetInstance()->myFprintf(0,fp,"$GPINS,");//tag,
	Util::GetInstance()->myFprintf(0,fp,"%02d%02d%06.3f,A,",gps->getHour(),gps->getMin(),gps->getSec());//time,status,
	Util::GetInstance()->myFprintf(0,fp,"%d%07.4f,N,%d%02.4f,E,",Gps::degX1MToDeg(latitude),Gps::degX1MToMin(latitude),
			Gps::degX1MToDeg(longitude),Gps::degX1MToMin(longitude));//latitude,longitude,
	Util::GetInstance()->myFprintf(0,fp,"%.2f,%.2f,",Gps::mpsToKnot(sqrtf(mpsSpeed.x*mpsSpeed.x + mpsSpeed.y*mpsSpeed.y)),
						Gps::speedToDegDirection(mpsSpeed.x,mpsSpeed.y));//speed,direction,
	Util::GetInstance()->myFprintf(0,fp,"%06d,",gps->getDate());//date
	Util::GetInstance()->myFprintf(0,fp,",,A*00");
	Util::GetInstance()->myFprintf(0,fp,"\r\n");
}

void Gains::printIns(FILE* fp,KalmanFilter* kf,ImuData* imuData,GpsData* gpsData){
	Quaternion mPos = kf->getMPos();
	Quaternion mpsSpeed = kf->getMpsSpeed();
	Quaternion attitude = kf->getAttitude();
	Quaternion acl = imuData->mpspsAcl;
	Quaternion gyro= imuData->rpsRate;
	Quaternion cmps = imuData->uTCmps;
	
	float pitch,role,heading;
	attitude.getRadPitchRollHeading(&pitch,&role,&heading);
	
	int min = Gps::GetInstance()->getMin();
	float sec = Gps::GetInstance()->getSec();
	
	Util::GetInstance()->myFprintf(0,stdout,"$GIFUL,");
	Util::GetInstance()->myFprintf(0,stdout,"%.1f,",60*min+sec);//time,
	Util::GetInstance()->myFprintf(0,stdout,"%.3f,%.3f,%.3f,",heading*180/M_PI,pitch*180/M_PI,role*180/M_PI);//heading,pitch,role,
	Util::GetInstance()->myFprintf(0,stdout,"%.3f,%.3f,%.3f,",acl.x,acl.y,acl.z);//acl
	Util::GetInstance()->myFprintf(0,stdout,"%.3f,%.3f,%.3f,",gyro.x,gyro.y,gyro.z);//gyro
	Util::GetInstance()->myFprintf(0,stdout,"%.3f,%.3f,%.3f,",cmps.x,cmps.y,cmps.z);//cmps
	Util::GetInstance()->myFprintf(0,stdout,"%.3f,%.3f,%.3f,",mpsSpeed.x,mpsSpeed.y,mpsSpeed.z);
	Util::GetInstance()->myFprintf(0,stdout,"%.3f,%.3f,%.3f,",mPos.x,mPos.y,mPos.z);
	Util::GetInstance()->myFprintf(0,stdout,"%.3f,%.3f,%.3f,",gpsData->mGpsRelativePos.x,gpsData->mGpsRelativePos.y,gpsData->mGpsRelativePos.z);
	Util::GetInstance()->myFprintf(0,stdout,"\r\n");
}
void Gains::printIns2(FILE* fp,ImuData* imuData){
	Util::GetInstance()->myFprintf(0,fp,"$GIRAW,");
	Util::GetInstance()->myFprintf(0,fp,"%.3f,%.3f,%.3f,",imuData->mpspsAcl.x,imuData->mpspsAcl.y,imuData->mpspsAcl.z);
	Util::GetInstance()->myFprintf(0,fp,"%.3f,%.3f,%.3f,",imuData->rpsRate.x,imuData->rpsRate.y,imuData->rpsRate.z);
	Util::GetInstance()->myFprintf(0,fp,"%.3f,%.3f,%.3f,",imuData->uTCmps.x,imuData->uTCmps.y,imuData->uTCmps.z);
	Util::GetInstance()->myFprintf(0,fp,"\r\n");
}
Quaternion Gains::toEarthFrame(Quaternion vec,Quaternion attitude){
	Quaternion attitudeCon;
	attitudeCon.con(&attitude);
	
	Quaternion earthFrameVec;
	vec.mul(&attitudeCon);
	attitude.mul(&vec);
	
	return attitude;
}

void Gains::appendInsData(ImuData* imuData){
	xQueueSend(imuQueue,imuData,0);
}
void Gains::appendGpsData(GpsData* gpsData){
	xQueueSend(gpsQueue,gpsData,0);
}


void Gains::resetImu(){
	//TODO: ここ排他制御する。
	Gps::GetInstance()->resetRefPosition();
	kf->reset();
}

/////////////////////////////////////
// getters
/////////////////////////////////////

/**
 * @brief 加速度を返す
 * 
 * 形式は[m/(s^2)]
 * 
 * クォータニオンをベクトルとして使用(w=0)
 * @return 現在の加速度を返す
 */
Quaternion Gains::getMpspsAcl(){
	Quaternion rt;
	xSemaphoreTake(dataUpdateMutex,portMAX_DELAY);
	rt = imuData.mpspsAcl;
	xSemaphoreGive(dataUpdateMutex);
	return rt;
}
/**
 * @brief 角速度を返す
 * 
 * 形式は[rad/s]
 * 
 * クォータニオンをベクトルとして使用(w=0)
 * @return 現在の角速度を返す 
 */
Quaternion Gains::getRpsRate(){
	Quaternion rt;
	xSemaphoreTake(dataUpdateMutex,portMAX_DELAY);
	rt = imuData.rpsRate;
	xSemaphoreGive(dataUpdateMutex);
	return rt;
}

/**
 * @brief EKFによって推定した現在の速度を返す
 * 
 * 形式は速度[m/s]
 * 
 * クォータニオンをベクトルとして使用(w=0)
 * 
 * 多くの場合(特に慣性センサとしてMPU-9250を使用する場合)、GPS単体から取得した距離の方が正確である
 * @return 現在の位置
 */
Quaternion Gains::getMpsSpeed(){
	Quaternion rt;
	xSemaphoreTake(dataUpdateMutex,portMAX_DELAY);
	rt = mpsSpeed;
	xSemaphoreGive(dataUpdateMutex);
	return rt;
}

/**
 * @brief EKFによって推定した現在の位置を返す
 * 
 * 形式は初期化時の位置からの距離[m]
 * 
 * クォータニオンをベクトルとして使用(w=0)
 * 
 * 多くの場合(特に慣性センサとしてMPU-9250を使用する場合)、GPS単体から取得した距離の方が正確である
 * @return 現在の位置
 */
Quaternion Gains::getMRelativePos(){
	Quaternion rt;
	xSemaphoreTake(dataUpdateMutex,portMAX_DELAY);
	rt = mRelativePos;
	xSemaphoreGive(dataUpdateMutex);
	return rt;
}

/**
 * @brief 現在の姿勢を返す
 * 
 * @return 現在の姿勢
 */
Quaternion Gains::getAttitude(){
	Quaternion rt;
	xSemaphoreTake(dataUpdateMutex,portMAX_DELAY);
	rt = attitude;
	xSemaphoreGive(dataUpdateMutex);
	return rt;
}

/**
 * @brief GPSの初期化時の座標を返す
 * 
 * 
 * 単位は[deg]*1000000。
 * 
 * 例：北緯35.123456°の場合、35123456を返す。
 * @return GPSの初期化時の経度。
 */
int Gains::getLattitudeRef(){
	return Gps::GetInstance()->mPosXToDegX1M_Latitude(0.0);
}

/**
 * @brief GPSの初期化時の座標を返す
 * 
 * 単位は[deg]*1000000。
 * 
 * 例：東経139.123456°の場合、139123456を返す。
 * @return GPSの初期化時の経度。
 */
int Gains::getLongitudeRef(){
	return Gps::GetInstance()->mPosYToDegX1M_Longitude(0.0);
}
/**
 * @brief 慣性センサからのデータを返す
 * 
 * @return 最新の慣性センサのデータ
 */
ImuData Gains::getImuData(){
	ImuData rt;
	xSemaphoreTake(dataUpdateMutex,portMAX_DELAY);
	rt = imuData;
	xSemaphoreGive(dataUpdateMutex);
	return rt;
}

/**
 * @brief GPSからのデータを返す
 * 
 * @return 最新のGPSデータ。位置は初期化時の位置からの距離[m]であることに注意。
 */
GpsData Gains::getGpsData(){
	GpsData rt;
	xSemaphoreTake(dataUpdateMutex,portMAX_DELAY);
	rt = gpsData;
	xSemaphoreGive(dataUpdateMutex);
	return rt;
}

void Gains::setPrintType(GainsPrintMode::Mode mode){
	xQueueSend(printModeQueue,&mode,0);
}

/**
 * @brief Gainsの関連タスクの起動、クラスの初期化を行う
 */
void Gains::initGains(){
	if(IMU_TYPE == ImuType::SELECTABLE){
		IMU_TYPE = Util::GetInstance()->flashData.imuType;
	}
	if(GPS_TYPE == GpsType::SELECTABLE){
		GPS_TYPE = Util::GetInstance()->flashData.gpsType;
	}
	
	//task
	xTaskCreate(&Gains::prvGainsTask,"gains",2048,NULL,3,&(Gains::GetInstance()->gainsHandle));
	
	if(IMU_TYPE == ImuType::ADIS16488){
		xTaskCreate(&ADIS16488::prvAdis16488Task,"adis",1024,NULL,4,&(Gains::GetInstance()->insHandle));
	}else if(IMU_TYPE == ImuType::MPU9250){
		xTaskCreate(prvMpu9250TaskEntry,"mpu",1024,NULL,4,&(Gains::GetInstance()->insHandle));
	}else if(IMU_TYPE == ImuType::MPU9250_BMP850){
		xTaskCreate(prvMpu9250TaskEntry,"mpubmp",1024,NULL,4,&(Gains::GetInstance()->insHandle));
	}
	if(GPS_TYPE == GpsType::USART_GPS){
		xTaskCreate(&USART2Class::prvTxTask,"gpstx",512,USART2,3,&(Gains::GetInstance()->gpsHandleTx));
		xTaskCreate(&USART2Class::prvRxTask,"gpsrx",1024,USART2,3,&(Gains::GetInstance()->gpsHandleRx));
	}else if(GPS_TYPE == GpsType::DUMMY_GPS){
		xTaskCreate(&DummyGps::prvDummyGpsTask,"dmygps",512,NULL,3,&(Gains::GetInstance()->gpsHandleRx));
	}
}
