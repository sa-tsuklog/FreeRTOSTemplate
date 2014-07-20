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
 * @brief GPS�̏��̎󂯓n���Ɏg�p����N���X
 * 
 * NED���W�n
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
	Quaternion mGpsRelativePos; /**<GPS���������̍��W����̑��Έʒu[m]�B�x�N�g���Ƃ��Ďg�p(w=0)*/
	Quaternion mpsGpsSpeed; /**<���x[m/s]�B�x�N�g���Ƃ��Ďg�p(w=0)*/
};


#endif /* GPSDATA_H_ */
