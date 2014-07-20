/*
 * TIM1.cpp
 *
 *  Created on: 2014/04/21
 *      Author: sa
 */

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "ServoTim1.h"

ServoTim1::ServoTim1(){
	/////////////////////////////////////
	//GPIO
	/////////////////////////////////////
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	GPIO_InitTypeDef gpioe9;
	GPIO_StructInit(&gpioe9);
	gpioe9.GPIO_Pin = GPIO_Pin_9;
	gpioe9.GPIO_Mode = GPIO_Mode_AF;
	gpioe9.GPIO_OType = GPIO_OType_PP;
	gpioe9.GPIO_Speed = GPIO_Speed_100MHz;
	gpioe9.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&gpioe9);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_TIM1);
	
	GPIO_InitTypeDef gpioe11;
	GPIO_StructInit(&gpioe11);
	gpioe11.GPIO_Pin = GPIO_Pin_11;
	gpioe11.GPIO_Mode = GPIO_Mode_AF;
	gpioe11.GPIO_OType = GPIO_OType_PP;
	gpioe11.GPIO_Speed = GPIO_Speed_100MHz;
	gpioe11.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&gpioe11);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_TIM1);
	
	GPIO_InitTypeDef gpioe13;
	GPIO_StructInit(&gpioe13);
	gpioe13.GPIO_Pin = GPIO_Pin_13;
	gpioe13.GPIO_Mode = GPIO_Mode_AF;
	gpioe13.GPIO_OType = GPIO_OType_PP;
	gpioe13.GPIO_Speed = GPIO_Speed_100MHz;
	gpioe13.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&gpioe13);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_TIM1);
	
	GPIO_InitTypeDef gpioe14;
	GPIO_StructInit(&gpioe14);
	gpioe14.GPIO_Pin = GPIO_Pin_14;
	gpioe14.GPIO_Mode = GPIO_Mode_AF;
	gpioe14.GPIO_OType = GPIO_OType_PP;
	gpioe14.GPIO_Speed = GPIO_Speed_100MHz;
	gpioe14.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&gpioe14);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_TIM1);
	
	/////////////////////////////////////
	//TIM
	/////////////////////////////////////
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	TIM_TimeBaseInitTypeDef timebase;
	TIM_TimeBaseStructInit(&timebase);
	timebase.TIM_ClockDivision = TIM_CKD_DIV1;
	timebase.TIM_Prescaler = 168-1;
	timebase.TIM_Period = 15000-1;
	timebase.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1,&timebase);
	
	TIM_OCInitTypeDef oc1def;
	TIM_OCStructInit(&oc1def);
	oc1def.TIM_OCMode = TIM_OCMode_PWM1;
	oc1def.TIM_OutputState = TIM_OutputState_Enable;
	oc1def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc1def.TIM_Pulse = 1500-1;
	TIM_OC1Init(TIM1,&oc1def);
	
	TIM_OCInitTypeDef oc2def;
	TIM_OCStructInit(&oc2def);
	oc2def.TIM_OCMode = TIM_OCMode_PWM1;
	oc2def.TIM_OutputState = TIM_OutputState_Enable;
	oc2def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc2def.TIM_Pulse = 1500-1;
	TIM_OC2Init(TIM1,&oc2def);
	
	TIM_OCInitTypeDef oc3def;
	TIM_OCStructInit(&oc3def);
	oc3def.TIM_OCMode = TIM_OCMode_PWM1;
	oc3def.TIM_OutputState = TIM_OutputState_Enable;
	oc3def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc3def.TIM_Pulse = 1500-1;
	TIM_OC3Init(TIM1,&oc3def);
	
	TIM_OCInitTypeDef oc4def;
	TIM_OCStructInit(&oc4def);
	oc4def.TIM_OCMode = TIM_OCMode_PWM1;
	oc4def.TIM_OutputState = TIM_OutputState_Enable;
	oc4def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc4def.TIM_Pulse = 1500-1;
	TIM_OC4Init(TIM1,&oc4def);
	
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM1,TIM_OCPreload_Enable);
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
}

void ServoTim1::timerStart(){
	TIM_Cmd(TIM1,ENABLE);
}
void ServoTim1::setDuty(int ch,uint32_t duty){
	switch(ch){
	case 1:TIM_SetCompare1(TIM1,duty);break;
	case 2:TIM_SetCompare2(TIM1,duty);break;
	case 3:TIM_SetCompare3(TIM1,duty);break;
	case 4:TIM_SetCompare4(TIM1,duty);break;
	}
}


