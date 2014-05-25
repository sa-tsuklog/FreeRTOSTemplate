/*
 * GpsData.h
 *
 *  Created on: 2014/05/25
 *      Author: sa
 */

#ifndef GPSDATA_H_
#define GPSDATA_H_

#include "../Util/Quaternion.h"

class GpsData{
private:
public:
	GpsData(float mGpsRelativePosX,float mGpsRelativePosY,float mGpsRelativePosZ,
			float mpsGpsSpeedX,float mpsGpsSpeedY,float mpsGpsSpeedZ){
		this->mGpsRelativePos = Quaternion(0,mGpsRelativePosX,mGpsRelativePosY,mGpsRelativePosZ);
		this->mpsGpsRelativeSpeed = Quaternion(0,mpsGpsSpeedX,mpsGpsSpeedY,mpsGpsSpeedZ);
	}
	Quaternion mGpsRelativePos;
	Quaternion mpsGpsRelativeSpeed;
};


#endif /* GPSDATA_H_ */
