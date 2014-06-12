/*
 * BridgeTim3.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#include "stm32f4xx.h"
#include "BridgeTim3.h"

BridgeTim3::BridgeTim3(){
	/////////////////////////////////////
	//GPIO
	/////////////////////////////////////
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef gpioa6;
	GPIO_StructInit(&gpioa6);
	gpioa6.GPIO_Pin = GPIO_Pin_6;
	gpioa6.GPIO_Mode = GPIO_Mode_AF;
	gpioa6.GPIO_OType = GPIO_OType_PP;
	gpioa6.GPIO_Speed = GPIO_Speed_100MHz;
	gpioa6.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&gpioa6);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3);
	
	GPIO_InitTypeDef gpioa7;
	GPIO_StructInit(&gpioa7);
	gpioa7.GPIO_Pin = GPIO_Pin_7;
	gpioa7.GPIO_Mode = GPIO_Mode_AF;
	gpioa7.GPIO_OType = GPIO_OType_PP;
	gpioa7.GPIO_Speed = GPIO_Speed_100MHz;
	gpioa7.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&gpioa7);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_TIM3);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef gpiob0;
	GPIO_StructInit(&gpiob0);
	gpiob0.GPIO_Pin = GPIO_Pin_0;
	gpiob0.GPIO_Mode = GPIO_Mode_AF;
	gpiob0.GPIO_OType = GPIO_OType_PP;
	gpiob0.GPIO_Speed = GPIO_Speed_100MHz;
	gpiob0.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&gpiob0);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3);
	
	GPIO_InitTypeDef gpiob1;
	GPIO_StructInit(&gpiob1);
	gpiob1.GPIO_Pin = GPIO_Pin_1;
	gpiob1.GPIO_Mode = GPIO_Mode_AF;
	gpiob1.GPIO_OType = GPIO_OType_PP;
	gpiob1.GPIO_Speed = GPIO_Speed_100MHz;
	gpiob1.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&gpiob1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_TIM3);
	
	/////////////////////////////////////
	//TIM
	/////////////////////////////////////
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_TimeBaseInitTypeDef timebase;
	TIM_TimeBaseStructInit(&timebase);
	timebase.TIM_ClockDivision = TIM_CKD_DIV1;
	timebase.TIM_Prescaler = 0;
	timebase.TIM_Period = 65535;
	timebase.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&timebase);
	
	TIM_OCInitTypeDef oc1def;
	TIM_OCStructInit(&oc1def);
	oc1def.TIM_OCMode = TIM_OCMode_PWM1;
	oc1def.TIM_OutputState = TIM_OutputState_Enable;
	oc1def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc1def.TIM_Pulse = 1000;
	TIM_OC1Init(TIM3,&oc1def);
	
	TIM_OCInitTypeDef oc2def;
	TIM_OCStructInit(&oc2def);
	oc2def.TIM_OCMode = TIM_OCMode_PWM1;
	oc2def.TIM_OutputState = TIM_OutputState_Enable;
	oc2def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc2def.TIM_Pulse = 2000;
	TIM_OC2Init(TIM3,&oc2def);
	
	TIM_OCInitTypeDef oc3def;
	TIM_OCStructInit(&oc3def);
	oc3def.TIM_OCMode = TIM_OCMode_PWM1;
	oc3def.TIM_OutputState = TIM_OutputState_Enable;
	oc3def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc3def.TIM_Pulse = 3000;
	TIM_OC3Init(TIM3,&oc3def);
	
	TIM_OCInitTypeDef oc4def;
	TIM_OCStructInit(&oc4def);
	oc4def.TIM_OCMode = TIM_OCMode_PWM1;
	oc4def.TIM_OutputState = TIM_OutputState_Enable;
	oc4def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc4def.TIM_Pulse = 4000;
	TIM_OC4Init(TIM3,&oc4def);
	
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);
	
}

void BridgeTim3::timerStart(){
	TIM_Cmd(TIM3,ENABLE);
}

void BridgeTim3::setDuty(int ch,unsigned short duty){
	if(ch==0){
		TIM_SetCompare1(TIM3,duty);
	}else if(ch == 1){
		TIM_SetCompare2(TIM3,duty);
	}else if(ch == 2){
		TIM_SetCompare3(TIM3,duty);
	}else if(ch == 3){
		TIM_SetCompare4(TIM3,duty);
	}
	
}
