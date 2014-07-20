/*
 * Gains.h
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */

#ifndef GAINS_H_
#define GAINS_H_

#include "Common/Quaternion.h"
#include "GainsConfig.h"
#include "ImuData.h"
#include "GpsData.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "KalmanFilter.h"
#include <stdio.h>
#include "semphr.h"


/**
 * @class Gains 
 * @brief EKF�ɂ��p������
 * 
 * �Z���T�͈ȉ��̂��̂���I���B
 * 
 *   �����Z���T�FADIS16488 or MPU-9250 or MPU-9250 + BMP850
 *   
 *   GPS:�V���A���ڑ���GPS or �_�~�[��GPS
 *   
 * �Z���T�̑I����GainsConfig.h�ōs���B
 * 
 * NED���W�n���g�p�B
 */

class Gains{
	// Singleton pattern definition
private:
	Gains();
	Gains(const Gains& rhs);
	Gains& operator=(const Gains& rhs);
	virtual ~Gains() {}
public:
	/**
	 * @brief ���̃N���X�̃C���X�^���X��Ԃ�
	 */
	static Gains* GetInstance() {
    	static Gains instance;
    	return &instance;
	}
	
	// Class definition
private:
	xQueueHandle imuQueue;
	xQueueHandle gpsQueue;
	xQueueHandle printModeQueue;
	
	SemaphoreHandle_t dataUpdateMutex;
	
	KalmanFilter* kf;
	GpsData gpsData;
	ImuData imuData;
	Quaternion attitude;
	Quaternion mpsSpeed;
	Quaternion mRelativePos;
	
	static const int MS_GPS_INITIAL_WAIT_TIME = 5000; //5sec
	static const int GPS_WATCHDOG_MAX = 1000; //ins predict 1000 times = about 10 sec 
	int gpsWatchDog;
	
	GainsPrintMode::Mode printMode;
	
	void print();
	void gainsTask(void *pvParameters);
public:
	void appendInsData(ImuData *imuData);
	void appendGpsData(GpsData *gpsData);
	
	Quaternion getMpspsAcl();
	Quaternion getMpsSpeed();
	Quaternion getMRelativePos();
	Quaternion getRpsRate();
	Quaternion getAttitude();
	int getLattitudeRef();
	int getLongitudeRef();
	ImuData getImuData();
	GpsData getGpsData();
	void setPrintType(GainsPrintMode::Mode mode);
	
	static void prvGainsTask(void *pvParameters);
	void printNMEA(FILE* fp,KalmanFilter* kf,GpsData* gpsData);
	void printIns(FILE* fp,KalmanFilter* kf,ImuData* imuData,GpsData* gpsData);
	void printIns2(FILE* fp,ImuData* imuData);
	Quaternion toEarthFrame(Quaternion vec,Quaternion attitude);
	
	void resetImu();
	
	static void initGains();
};



#endif


