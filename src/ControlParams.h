/*
 * ControlParams.h
 *
 *  Created on: 2014/07/05
 *      Author: sa
 */

#ifndef CONTROLPARAMS_H_
#define CONTROLPARAMS_H_

class ControlParams{
public:
	ControlParams(unsigned char mode,
			unsigned char pitch,
			unsigned char roll,
			unsigned char yaw,
			unsigned char rz,
			unsigned char throttle,
			short cameraH,
			short cameraV,
			unsigned char reserved,
			unsigned char camMode){
		this->mode = mode;
		this->pitch = pitch;
		this->roll = roll;
		this->yaw = yaw;
		this->rz  = rz;
		this->throttle = throttle;
		this->cameraH = cameraH;
		this->cameraV = cameraV;
		this->reserved = reserved;
		this->camMode = camMode;
	}
	unsigned char mode;
	unsigned char pitch;
	unsigned char roll;
	unsigned char yaw;
	unsigned char rz;
	unsigned char throttle;
	short cameraH;
	short cameraV;
	unsigned char reserved;
	unsigned char camMode;
	
	static const int BIT_CAM_MODE_STABILIZE = 0xC0;
};

#endif /* CONTROLPARAMS_H_ */
