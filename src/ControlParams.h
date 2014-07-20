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
	ControlParams(unsigned char pitch,unsigned char role,unsigned char yaw,
			unsigned char throttle,unsigned char flaps){
		this->pitch    = pitch;
		this->role     = role;
		this->yaw      = yaw;
		this->throttle = throttle;
		this->flaps    = flaps;
	}
	unsigned char pitch;
	unsigned char role;
	unsigned char yaw;
	unsigned char throttle;
	unsigned char flaps;
};

#endif /* CONTROLPARAMS_H_ */
