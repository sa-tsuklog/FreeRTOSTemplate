/*
 * ImuData.h
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */

#ifndef IMUDATA_H_
#define IMUDATA_H_

#include "../Util/Quaternion.h"

class ImuData{
private:
public:
	ImuData(float rpsRateX,float rpsRateY,float rpsRateZ,
			float mpspsAclX,float mpspsAclY,float mpspsAclZ,
			float uTCmpsX,float uTCmpsY,float uTCmpsZ,
			float prsPressure,
			int isCmpsValid,int isPressureValid){
		this->rpsRate = Quaternion(0,rpsRateX,rpsRateY,rpsRateZ);
		this->mpspsAcl = Quaternion(0,mpspsAclX,mpspsAclY,mpspsAclZ);
		this->uTCmps   = Quaternion(0,uTCmpsX,uTCmpsY,uTCmpsZ);
		this->prsPressure = prsPressure;
		this->isCmpsValid = isCmpsValid;
		this->isPressureValid= isPressureValid;
	}
	Quaternion mpspsAcl;
	Quaternion rpsRate;
	Quaternion uTCmps;
	float prsPressure;
	int isCmpsValid;
	int isPressureValid;
};


#endif /* IMUDATA_H_ */
