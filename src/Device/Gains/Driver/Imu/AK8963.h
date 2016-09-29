/*
 * AK8963.h
 *
 *  Created on: 2016/09/28
 *      Author: sa
 */

#ifndef AK8963_H_
#define AK8963_H_

#include "stdint.h"

class Ak8963 {
	// Singleton pattern definition
private:
	Ak8963();
	Ak8963(const Ak8963& rhs);
	Ak8963& operator=(const Ak8963& rhs);
	virtual ~Ak8963() {}
public:
	static Ak8963* GetInstance() {
		static Ak8963 instance;
		return &instance;
	}

	// Class definition
private:
	void write8(uint8_t address,uint8_t data);
	uint8_t read8(uint8_t address);
	void read48(uint8_t address,uint8_t* buf);
	
	uint8_t asa[3];
public:
	void init();
	void readUtCmps(float* buf);
};

#endif /* AK8963_H_ */
