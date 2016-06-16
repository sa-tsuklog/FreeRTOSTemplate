/*
 * ImuData.h
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */

#ifndef IMUDATA_H_
#define IMUDATA_H_

#include "Common/Math/Quaternion.h"

/**
 * @class ImuData
 * 
 * @brief 慣性センサのデータ受け渡しに使用するクラス
 * 
 * 機体座標系
 */

class ImuData{
private:
public:
	ImuData(float rpsRateX,float rpsRateY,float rpsRateZ,
			float mpspsAclX,float mpspsAclY,float mpspsAclZ,
			float uTCmpsX,float uTCmpsY,float uTCmpsZ,
			float paPressure,
			int isCmpsValid,int isPressureValid,
			float degTemp){
		this->rpsRate = Quaternion(0,rpsRateX,rpsRateY,rpsRateZ);
		this->mpspsAcl = Quaternion(0,mpspsAclX,mpspsAclY,mpspsAclZ);
		this->uTCmps   = Quaternion(0,uTCmpsX,uTCmpsY,uTCmpsZ);
		this->paPressure = paPressure;
		this->isCmpsValid = isCmpsValid;
		this->isPressureValid= isPressureValid;
		this->degTemp = degTemp;
	}
	ImuData(){
		this->rpsRate = Quaternion(0,0,0,0);
		this->mpspsAcl= Quaternion(0,0,0,0);
		this->uTCmps  = Quaternion(0,0,0,0);
		this->paPressure=0;
		this->isCmpsValid=0;
		this->isPressureValid=0;
		this->degTemp = degTemp;
	}
	Quaternion mpspsAcl; /**<加速度[m/(s^2)]。ベクトルとして使用(w=0)*/
	Quaternion rpsRate;  /**<角速度[rad/s]。ベクトルとして使用(w=0)*/
	Quaternion uTCmps;   /**<磁気[uT]。ベクトルとして使用*/
	float paPressure;    /**<気圧[pa]*/
	int isCmpsValid;     /**<磁気のデータに更新があった場合1、それ以外の場合0*/
	int isPressureValid; /**<気圧のデータに更新があった場合1、それ以外の場合0*/
	float degTemp;	/**<センサ温度[deg]*/
	
	static float paToRelativeHeight(float paPressure,float paRefPressure);
};


#endif /* IMUDATA_H_ */
