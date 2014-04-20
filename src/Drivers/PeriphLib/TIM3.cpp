/*
 * HalTim.c
 *
 *  Created on: 2014/02/27
 *      Author: sa
 */

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "TIM3.h"

TIM3Class::TIM3Class(){
	/////////////////////////////////////
	//GPIO
	/////////////////////////////////////
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef gpiob5;
	GPIO_StructInit(&gpiob5);
	gpiob5.GPIO_Pin = GPIO_Pin_5;
	gpiob5.GPIO_Mode = GPIO_Mode_AF;
	gpiob5.GPIO_OType = GPIO_OType_PP;
	gpiob5.GPIO_Speed = GPIO_Speed_100MHz;
	gpiob5.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&gpiob5);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_TIM3);
	
	
	
	
	/////////////////////////////////////
	//TIM
	/////////////////////////////////////
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_TimeBaseInitTypeDef timebase;
	TIM_TimeBaseStructInit(&timebase);
	timebase.TIM_ClockDivision = TIM_CKD_DIV1;
	timebase.TIM_Prescaler = 84-1;
	timebase.TIM_Period = 15000-1;
	timebase.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&timebase);
	
	TIM_OCInitTypeDef oc3def;
	TIM_OCStructInit(&oc3def);
	oc3def.TIM_OCMode = TIM_OCMode_PWM1;
	oc3def.TIM_OutputState = TIM_OutputState_Enable;
	oc3def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc3def.TIM_Pulse = 1500-4;
	TIM_OC1Init(TIM3,&oc3def);
	
	TIM_OCInitTypeDef oc1def;
	TIM_OCStructInit(&oc1def);
	oc1def.TIM_OCMode = TIM_OCMode_PWM1;
	oc1def.TIM_OutputState = TIM_OutputState_Enable;
	oc1def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc1def.TIM_Pulse = 1500-4;
	TIM_OC1Init(TIM3,&oc1def);
	
	TIM_OCInitTypeDef oc2def;
	TIM_OCStructInit(&oc2def);
	oc2def.TIM_OCMode = TIM_OCMode_PWM1;
	oc2def.TIM_OutputState = TIM_OutputState_Enable;
	oc2def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc2def.TIM_Pulse = 1500-4;
	TIM_OC2Init(TIM3,&oc2def);
	
	TIM_OCInitTypeDef oc4def;
	TIM_OCStructInit(&oc4def);
	oc4def.TIM_OCMode = TIM_OCMode_PWM1;
	oc4def.TIM_OutputState = TIM_OutputState_Enable;
	oc4def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc4def.TIM_Pulse = 1500-4;
	TIM_OC4Init(TIM3,&oc4def);
}

void TIM3Class::timerStart(){
	TIM_Cmd(TIM3,ENABLE);
}
