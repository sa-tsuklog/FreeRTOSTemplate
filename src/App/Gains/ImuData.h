/*
 * ImuData.h
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */

#ifndef IMUDATA_H_
#define IMUDATA_H_

class ImuData{
private:
public:
	ImuData(float rpsRateX,float rpsRateY,float rpsRateZ,
			float mpspsAclX,float mpspsAclY,float mpspsAclZ,
			float uTCmpsX,float uTCmpsY,float uTCmpsZ,
			float prsPressure,
			int isCmpsValid,int isPressureValid){
		this->rpsRate[0]  = rpsRateX;
		this->rpsRate[1]  = rpsRateY;
		this->rpsRate[2]  = rpsRateZ;
		this->mpspsAcl[0] = mpspsAclX;
		this->mpspsAcl[1] = mpspsAclY;
		this->mpspsAcl[2] = mpspsAclZ;
		this->uTCmps[0]   = uTCmpsX;
		this->uTCmps[1]   = uTCmpsY;
		this->uTCmps[2]   = uTCmpsZ;
		this->prsPressure = prsPressure;
		this->isCmpsValid = isCmpsValid;
		this->isPressureValid= isPressureValid;
	}
	float mpspsAcl[3];
	float rpsRate[3];
	float uTCmps[3];
	float prsPressure;
	int isCmpsValid;
	int isPressureValid;
};


#endif /* IMUDATA_H_ */
