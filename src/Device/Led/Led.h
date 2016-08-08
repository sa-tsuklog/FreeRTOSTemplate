/*
 * Led.h
 *
 *  Created on: 2016/07/17
 *      Author: sa
 */

#ifndef LED_H_
#define LED_H_

#include "stdint.h"

class Led {
	// Singleton pattern definition
private:
	Led();
	Led(const Led& rhs);
	Led& operator=(const Led& rhs);
	virtual ~Led() {}
public:
	static Led* GetInstance() {
		static Led instance;
		return &instance;
	}

	// Class definition
private:
	uint32_t forceOn;
	
public:
	void set(uint32_t ont);
	void force(uint32_t on);
	void release();
};

#endif /* LED_H_ */
