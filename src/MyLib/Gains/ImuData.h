/*
 * ImuData.h
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */

#ifndef IMUDATA_H_
#define IMUDATA_H_

#include "Common/Quaternion.h"

/**
 * @class ImuData
 * 
 * @brief �����Z���T�̃f�[�^�󂯓n���Ɏg�p����N���X
 * 
 * �@�̍��W�n
 */

class ImuData{
private:
public:
	ImuData(float rpsRateX,float rpsRateY,float rpsRateZ,
			float mpspsAclX,float mpspsAclY,float mpspsAclZ,
			float uTCmpsX,float uTCmpsY,float uTCmpsZ,
			float paPressure,
			int isCmpsValid,int isPressureValid){
		this->rpsRate = Quaternion(0,rpsRateX,rpsRateY,rpsRateZ);
		this->mpspsAcl = Quaternion(0,mpspsAclX,mpspsAclY,mpspsAclZ);
		this->uTCmps   = Quaternion(0,uTCmpsX,uTCmpsY,uTCmpsZ);
		this->paPressure = paPressure;
		this->isCmpsValid = isCmpsValid;
		this->isPressureValid= isPressureValid;
	}
	ImuData(){
		this->rpsRate = Quaternion(0,0,0,0);
		this->mpspsAcl= Quaternion(0,0,0,0);
		this->uTCmps  = Quaternion(0,0,0,0);
		this->paPressure=0;
		this->isCmpsValid=0;
		this->isPressureValid=0;
	}
	Quaternion mpspsAcl; /**<�����x[m/(s^2)]�B�x�N�g���Ƃ��Ďg�p(w=0)*/
	Quaternion rpsRate;  /**<�p���x[rad/s]�B�x�N�g���Ƃ��Ďg�p(w=0)*/
	Quaternion uTCmps;   /**<���C[uT]�B�x�N�g���Ƃ��Ďg�p*/
	float paPressure;    /**<�C��[pa]*/
	int isCmpsValid;     /**<���C�̃f�[�^�ɍX�V���������ꍇ1�A����ȊO�̏ꍇ0*/
	int isPressureValid; /**<�C���̃f�[�^�ɍX�V���������ꍇ1�A����ȊO�̏ꍇ0*/
	
	static float paToRelativeHeight(float paPressure,float paRefPressure);
};


#endif /* IMUDATA_H_ */
