/*
 * Util.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */


#include "Util.h"
#include "Driver/ADC3.h"
#include "Driver/TIM2.h"
#include "Driver/USER_FLASH.h"

Util::Util(){
	//TIM2 initialize
	TIM2Class::GetInstance()->timerStart();
	
	//ADC2 initialize
	ADC3Class::GetInstance()->startFirstConversion();
	
	//USER_FLASH initialize
	//TODO ‚Â‚­‚é
	
	//task
	//no tasks.
}

uint32_t Util::getUsTime(){
	TIM2Class::GetInstance()->getUsTime();
}
float Util::getVoltInputVoltage(){
	ADC3Class::GetInstance()->getNextData();
}
void Util::userflashFlush(){
	FLASHClass::GetInstance()->flush();
}

static void Util::initUtil(){
	Util::GetInstance();
}
