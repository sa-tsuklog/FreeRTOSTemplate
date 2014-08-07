/*
 * Util.h
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */
#ifndef UTIL_H_
#define UTIL_H_

#include "FreeRTOS.h"
#include "semphr.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "UserFlashData.h"


/**
 * @class Util
 * @brief 便利関数いろいろ
 */
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
	SemaphoreHandle_t printfMutex;
	
	void initFlashData();
public:
	UserflashData flashData; 
	
	uint32_t getUsTime();
	float getVoltInputVoltage();
	void userflashFlush();
	void myFprintf(portTickType blockTime,FILE* stream,const char* format,...);
	static void initUtil();
};




#endif
