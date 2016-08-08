/*
 * LedGpio.h
 *
 *  Created on: 2016/07/17
 *      Author: sa
 */

#ifndef LEDGPIO_H_
#define LEDGPIO_H_

#include "stdint.h"

class LedGpio {
	// Singleton pattern definition
private:
	LedGpio();
	LedGpio(const LedGpio& rhs);
	LedGpio& operator=(const LedGpio& rhs);
	virtual ~LedGpio() {}
public:
	static LedGpio* GetInstance() {
		static LedGpio instance;
		return &instance;
	}

	// Class definition
private:
	
public:
	void set(uint32_t on);
	
};

#endif /* LEDGPIO_H_ */
