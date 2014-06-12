/*
 * Util.h
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */
#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>
#include "UserFlashData.h"


class Util {
	// Singleton pattern definition
private:
	Util();
	Util(const Util& rhs);
	Util& operator=(const Util& rhs);
	virtual ~Util() {}
public:
	static Util* GetInstance() {
    	static Util instance;
    	return &instance;
	}

	// Class definition
private:
public:
	UserflashData flashData; 
	
	uint32_t getUsTime();
	float getVoltInputVoltage();
	void userflashFlush();
	
	static void initUtil();
};




#endif
