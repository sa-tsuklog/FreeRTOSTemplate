/*
 * Led.cpp
 *
 *  Created on: 2016/07/17
 *      Author: sa
 */

#include "Led.h"
#include "Driver/LedGpio.h"

Led::Led(){
	forceOn = 0;
}

void Led::set(uint32_t on){
	if(forceOn == 0){
		LedGpio::GetInstance()->set(on);
	}
}

void Led::force(uint32_t on){
	forceOn = 1;
	LedGpio::GetInstance()->set(on);
}

void Led::release(){
	forceOn = 0;
}
