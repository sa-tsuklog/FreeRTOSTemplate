/*
 * TIM8.cpp
 *
 *  Created on: 2014/04/21
 *      Author: sa
 */

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "TIM8.h"

TIM8Class::TIM8Class(){
	/////////////////////////////////////
	//GPIO
	/////////////////////////////////////
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	GPIO_InitTypeDef gpioc6;
	GPIO_StructInit(&gpioc6);
	gpioc6.GPIO_Pin = GPIO_Pin_6;
	gpioc6.GPIO_Mode = GPIO_Mode_AF;
	gpioc6.GPIO_OType = GPIO_OType_PP;
	gpioc6.GPIO_Speed = GPIO_Speed_100MHz;
	gpioc6.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&gpioc6);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8);
	
	GPIO_InitTypeDef gpioc7;
	GPIO_StructInit(&gpioc7);
	gpioc7.GPIO_Pin = GPIO_Pin_7;
	gpioc7.GPIO_Mode = GPIO_Mode_AF;
	gpioc7.GPIO_OType = GPIO_OType_PP;
	gpioc7.GPIO_Speed = GPIO_Speed_100MHz;
	gpioc7.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&gpioc7);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8);
	
	GPIO_InitTypeDef gpioc8;
	GPIO_StructInit(&gpioc8);
	gpioc8.GPIO_Pin = GPIO_Pin_8;
	gpioc8.GPIO_Mode = GPIO_Mode_AF;
	gpioc8.GPIO_OType = GPIO_OType_PP;
	gpioc8.GPIO_Speed = GPIO_Speed_100MHz;
	gpioc8.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&gpioc8);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM8);
	
	GPIO_InitTypeDef gpioc9;
	GPIO_StructInit(&gpioc9);
	gpioc9.GPIO_Pin = GPIO_Pin_9;
	gpioc9.GPIO_Mode = GPIO_Mode_AF;
	gpioc9.GPIO_OType = GPIO_OType_PP;
	gpioc9.GPIO_Speed = GPIO_Speed_100MHz;
	gpioc9.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&gpioc9);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM8);
	
	/////////////////////////////////////
	//TIM
	/////////////////////////////////////
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
	TIM_TimeBaseInitTypeDef timebase;
	TIM_TimeBaseStructInit(&timebase);
	timebase.TIM_ClockDivision = TIM_CKD_DIV1;
	timebase.TIM_Prescaler = 168-1;
	timebase.TIM_Period = 15000-1;
	timebase.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM8,&timebase);
	
	TIM_OCInitTypeDef oc1def;
	TIM_OCStructInit(&oc1def);
	oc1def.TIM_OCMode = TIM_OCMode_PWM1;
	oc1def.TIM_OutputState = TIM_OutputState_Enable;
	oc1def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc1def.TIM_Pulse = 1500-4;
	TIM_OC1Init(TIM8,&oc1def);
	
	TIM_OCInitTypeDef oc2def;
	TIM_OCStructInit(&oc2def);
	oc2def.TIM_OCMode = TIM_OCMode_PWM1;
	oc2def.TIM_OutputState = TIM_OutputState_Enable;
	oc2def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc2def.TIM_Pulse = 1600-4;
	TIM_OC2Init(TIM8,&oc2def);
	
	TIM_OCInitTypeDef oc3def;
	TIM_OCStructInit(&oc3def);
	oc3def.TIM_OCMode = TIM_OCMode_PWM1;
	oc3def.TIM_OutputState = TIM_OutputState_Enable;
	oc3def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc3def.TIM_Pulse = 1700-4;
	TIM_OC3Init(TIM8,&oc3def);
	
	TIM_OCInitTypeDef oc4def;
	TIM_OCStructInit(&oc4def);
	oc4def.TIM_OCMode = TIM_OCMode_PWM1;
	oc4def.TIM_OutputState = TIM_OutputState_Enable;
	oc4def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc4def.TIM_Pulse = 1800-4;
	TIM_OC4Init(TIM8,&oc4def);
	
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
}

void TIM8Class::timerStart(){
	TIM_Cmd(TIM8,ENABLE);
}



