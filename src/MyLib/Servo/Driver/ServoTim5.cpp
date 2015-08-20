/*
 * TIM5.cpp
 *
 *  Created on: 2014/04/21
 *      Author: sa
 */

#include <pch.h>
#include "ServoTim5.h"

ServoTim5::ServoTim5(){
	/////////////////////////////////////
	//GPIO
	/////////////////////////////////////
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef gpioa1;
	GPIO_StructInit(&gpioa1);
	gpioa1.GPIO_Pin = GPIO_Pin_1;
	gpioa1.GPIO_Mode = GPIO_Mode_AF;
	gpioa1.GPIO_OType = GPIO_OType_PP;
	gpioa1.GPIO_Speed = GPIO_Speed_100MHz;
	gpioa1.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&gpioa1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5);
	
	
	/////////////////////////////////////
	//TIM
	/////////////////////////////////////
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	TIM_TimeBaseInitTypeDef timebase;
	TIM_TimeBaseStructInit(&timebase);
	timebase.TIM_ClockDivision = TIM_CKD_DIV1;
	timebase.TIM_Prescaler = 84-1;
	timebase.TIM_Period = 15000-1;
	timebase.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5,&timebase);
	
	TIM_OCInitTypeDef oc2def;
	TIM_OCStructInit(&oc2def);
	oc2def.TIM_OCMode = TIM_OCMode_PWM1;
	oc2def.TIM_OutputState = TIM_OutputState_Enable;
	oc2def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc2def.TIM_Pulse = 1500-1;
	TIM_OC2Init(TIM5,&oc2def);
	
	TIM_OC2PreloadConfig(TIM5,TIM_OCPreload_Enable);
}

void ServoTim5::timerStart(){
	TIM_Cmd(TIM5,ENABLE);
}

void ServoTim5::setDuty(int ch,uint32_t duty){
	switch(ch){
	case 2:TIM_SetCompare2(TIM5,duty);break;
	}
}
