/*
 * FpgaSeeker.h
 *
 *  Created on: 2016/09/22
 *      Author: sa
 */

#ifndef FPGASEEKER_H_
#define FPGASEEKER_H_

#include "stdint.h"

class FpgaSeeker {
	// Singleton pattern definition
private:
	FpgaSeeker();
	FpgaSeeker(const FpgaSeeker& rhs);
	FpgaSeeker& operator=(const FpgaSeeker& rhs);
	virtual ~FpgaSeeker() {}
public:
	static FpgaSeeker* GetInstance() {
		static FpgaSeeker instance;
		return &instance;
	}
	
	// Class definition
private:
	static const uint8_t ADDR_CH0 = 0x00;
	static const uint8_t ADDR_CH1 = 0x01;
	static const uint8_t ADDR_CH2 = 0x02;
	static const uint8_t ADDR_CH3 = 0x03;
	static const uint8_t ADDR_SCRATCH0 = 0x04;
	static const uint8_t ADDR_SCRATCH1 = 0x05;
	static const uint8_t ADDR_SCRATCH2 = 0x06;
	static const uint8_t ADDR_SCRATCH3 = 0x07;
	
	float upDown;
	float leftRight;
	float intensity;
	
	void FpgaSeekerTask();
public:
	void write32(uint8_t address,uint32_t value);
	uint32_t read32(uint8_t address);
	static void FpgaSeekerTaskEntry(void *pvParameters);
	static void initFpgaSeeker();
};


#endif /* FPGASEEKER_H_ */
