/*
 * LPS25HB.h
 *
 *  Created on: 2016/09/28
 *      Author: sa
 */

#ifndef LPS25HB_H_
#define LPS25HB_H_

#include "stdint.h"

class Lps25Hb {
	// Singleton pattern definition
private:
	Lps25Hb();
	Lps25Hb(const Lps25Hb& rhs);
	Lps25Hb& operator=(const Lps25Hb& rhs);
	virtual ~Lps25Hb() {}
public:
	static Lps25Hb* GetInstance() {
		static Lps25Hb instance;
		return &instance;
	}

	// Class definition
private:
	void write8(uint8_t address, uint8_t data);
	uint32_t read24(uint8_t address);
	uint16_t read16(uint8_t address);
	uint8_t read8(uint8_t address);
public:
	void init();
	float readPaPressure();
	float readDegTemp();
};

#endif /* LPS25HB_H_ */
