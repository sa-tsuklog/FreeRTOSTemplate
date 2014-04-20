/*
 * Adis16488.hpp
 *
 *  Created on: 2014/02/19
 *      Author: sa
 */

#ifndef ADIS16488_HPP_
#define ADIS16488_HPP_

#include "math.h"

class ADIS16488 {
	// Singleton pattern definition
private:
	ADIS16488();
	ADIS16488(const ADIS16488& rhs);
	ADIS16488& operator=(const ADIS16488& rhs);
	virtual ~ADIS16488() {}
public:
	static ADIS16488* GetInstance() {
    	static ADIS16488 instance;
    	return &instance;
	}

	// Class definition
private:
	static const float RPS_PER_LSB = M_PI/180.0*0.02;
	static const float RPS_PER_LSB_LOW = M_PI/180.0*0.02/65536;
	static const float MPSPS_PER_LSB = 9.8*0.0008;
	static const float MPSPS_PER_LSB_LOW = 9.8*0.0008/65536;
	static const float UT_PER_LSB = 0.0001*100;	//1m gauss/LSB, 1gauss = 100uT
	static const float PA_PER_LSB = 0.000040 * 100000;	//40ubar/LSB, 1bar = 100000pa
	static const int MAGN_VALID_MASK = 0x0100;
	static const int PRESSURE_VALID_MASK = 0x0200;
	
	static const float RPS_GYROBIAS_X = 1.3641236412363936E-5;
	static const float RPS_GYROBIAS_Y = -1.42741047410475E-4;
	static const float RPS_GYROBIAS_Z = -2.584670446704445E-4;
	
	
	void adis16488init();
	void setContinuousRead();
public:
	static void prvAdis16488Task(void *pvParameters);
	
};


#endif /* ADIS16488_HPP_ */
