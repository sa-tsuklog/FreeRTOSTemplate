/*
 * GpsData.h
 *
 *  Created on: 2014/05/25
 *      Author: sa
 */

#ifndef GPSDATA_H_
#define GPSDATA_H_

#include "Common/Quaternion.h"

/**
 * @class GpsData
 * 
 * @brief GPSの情報の受け渡しに使用するクラス
 * 
 * NED座標系
 */
class GpsData{
private:
public:
	GpsData(float mGpsRelativePosX,float mGpsRelativePosY,float mGpsRelativePosZ,
			float mpsGpsSpeedX,float mpsGpsSpeedY,float mpsGpsSpeedZ){
		this->mGpsRelativePos = Quaternion(0,mGpsRelativePosX,mGpsRelativePosY,mGpsRelativePosZ);
		this->mpsGpsSpeed = Quaternion(0,mpsGpsSpeedX,mpsGpsSpeedY,mpsGpsSpeedZ);
	}
	GpsData(){
		this->mGpsRelativePos = Quaternion(0,0,0,0);
		this->mpsGpsSpeed = Quaternion(0,0,0,0);
	}
	Quaternion mGpsRelativePos; /**<GPS初期化時の座標からの相対位置[m]。ベクトルとして使用(w=0)*/
	Quaternion mpsGpsSpeed; /**<速度[m/s]。ベクトルとして使用(w=0)*/
};


#endif /* GPSDATA_H_ */
