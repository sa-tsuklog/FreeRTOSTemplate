/*
 * TIM2.cpp
 *
 *  Created on: 2014/04/20
 *      Author: sa
 */
#include <pch.h>
#include "TIM2.h"

//for time measurement
TIM2Class::TIM2Class(){
	/////////////////////////////////////
	//TIM
	/////////////////////////////////////
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_TimeBaseInitTypeDef timebase;
	TIM_TimeBaseStructInit(&timebase);
	timebase.TIM_ClockDivision = TIM_CKD_DIV1;
	timebase.TIM_Prescaler = 84-1;
	timebase.TIM_Period = 0xFFFFFFFF;
	timebase.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&timebase);
}

void TIM2Class::timerStart(){
	TIM_Cmd(TIM2,ENABLE);
}

uint32_t TIM2Class::getUsTime(){
	return TIM_GetCounter(TIM2);
}
