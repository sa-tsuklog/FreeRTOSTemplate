/*
 * ICM20601.h
 *
 *  Created on: 2016/09/28
 *      Author: sa
 */

#ifndef ICM20601_H_
#define ICM20601_H_

#include "stdint.h"

class Icm20601 {
	// Singleton pattern definition
private:
	Icm20601();
	Icm20601(const Icm20601& rhs);
	Icm20601& operator=(const Icm20601& rhs);
	virtual ~Icm20601() {}
public:
	static Icm20601* GetInstance() {
		static Icm20601 instance;
		return &instance;
	}

	// Class definition
private:
	static const float MPSPS_PER_LSB = 9.798*16.0/(65536/2);
	static const float RPS_PER_LSB		=500*3.1415/180/(65536/2);
	static const float DEG_PER_LSB      =1.0/333.87;
	static const float DEG_TEMP_OFFSET  =21.0;
	
	void write8(uint8_t address,uint8_t data);
	uint8_t read8(uint8_t address);
	void read112(uint8_t address,uint8_t* buf);
public:
	void init();
	void readImu(float* mpspsAcl,float* rpsGyro,float* degTemp);
};

#endif /* ICM20601_H_ */
