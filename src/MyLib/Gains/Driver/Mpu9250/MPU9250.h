/*
 * HalI2c2.hpp
 *
 *  Created on: 2014/02/23
 *      Author: sa
 */

#ifndef MPU_9250_H_
#define MPU_9250_H_

#include "FreeRTOS.h"
#include "semphr.h"

class Mpu9250 {
	// Singleton pattern definition
private:
	Mpu9250();
	Mpu9250(const Mpu9250& rhs);
	Mpu9250& operator=(const Mpu9250& rhs);
	virtual ~Mpu9250() {}
public:
	static Mpu9250* getInstance() {
    	static Mpu9250 instance;
    	return &instance;
	}

	// Class definition
private:
	static const float MPSPS_PER_LSB	=9.8*16.0/(65536/2);
	static const float RPS_PER_LSB		=500*3.1415/180/(65536/2);
	static const float UT_PER_LSB		=4800.0/(65536/2);
	static const float DEG_PER_LSB      =1.0/333.87;
	static const float DEG_TEMP_OFFSET      =21.0;
	float cmpsGain[3];
	
	static const int SAMPLES_FOR_CALIBRATION = 500;
	float calibrationBuf[3];
	
	float mpspsAcl[3];
	float rpsRate[3];
	float uTCmps[3];
	float degTemp;
	float paPressure;
	
	SemaphoreHandle_t gyroCalibrationSem;
	SemaphoreHandle_t aclCalibrationSem;
public:
	void init();
	void waitNewData();
	void readMpu9250();
	
	void startGyroCalibration();
	void startAclCalibration();
	
	void prvMpu9250Task(void *pvParameters);
};



void prvMpu9250TaskEntry(void *pvParameters);

#endif /* HALI2C2_HPP_ */
