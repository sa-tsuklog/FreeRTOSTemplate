/*
 * UserFlashData.h
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#ifndef USERFLASHDATA_H_
#define USERFLASHDATA_H_

#include "Device/Gains/GainsConfig.h"
#include "Device/Servo/Servo.h"
#include "App/GliderControl/FlightParameters.h"

typedef struct{
	ImuType::Type imuType;
	GpsType::Type gpsType;
	float mpuGyroBias[3];
	float mpuAclBias[3];
	float mpuCmpsBias[3];
	float adisCmpsBias[3];
	int servoTrim[Servo::CH_NUM];
	float pGain[3];
	float dGain[3];
	float iGain[3];
	float mpuGyroTempCoefficient[3];
	float mpuAclTempCoefficient[3];
	float mpuCmpsTempCoefficient[3];
	FlightParameters flightParameters;
	float mpuCmpsMagnitude;
}UserflashData;

#endif /* USERFLASHDATA_H_ */
