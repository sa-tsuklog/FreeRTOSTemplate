/*
 * TIM4.cpp
 *
 *  Created on: 2014/04/21
 *      Author: sa
 */


#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "ServoTim4.h"
#include "stdio.h"

ServoTim4::ServoTim4(){
	/////////////////////////////////////
	//GPIO
	/////////////////////////////////////
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	GPIO_InitTypeDef gpiod12;
	GPIO_StructInit(&gpiod12);
	gpiod12.GPIO_Pin = GPIO_Pin_12;
	gpiod12.GPIO_Mode = GPIO_Mode_AF;
	gpiod12.GPIO_OType = GPIO_OType_PP;
	gpiod12.GPIO_Speed = GPIO_Speed_100MHz;
	gpiod12.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&gpiod12);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);
	
	GPIO_InitTypeDef gpiod13;
	GPIO_StructInit(&gpiod13);
	gpiod13.GPIO_Pin = GPIO_Pin_13;
	gpiod13.GPIO_Mode = GPIO_Mode_AF;
	gpiod13.GPIO_OType = GPIO_OType_PP;
	gpiod13.GPIO_Speed = GPIO_Speed_100MHz;
	gpiod13.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&gpiod13);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4);
	
	GPIO_InitTypeDef gpiod14;
	GPIO_StructInit(&gpiod14);
	gpiod14.GPIO_Pin = GPIO_Pin_14;
	gpiod14.GPIO_Mode = GPIO_Mode_AF;
	gpiod14.GPIO_OType = GPIO_OType_PP;
	gpiod14.GPIO_Speed = GPIO_Speed_100MHz;
	gpiod14.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&gpiod14);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_TIM4);
	
	GPIO_InitTypeDef gpiod15;
	GPIO_StructInit(&gpiod15);
	gpiod15.GPIO_Pin = GPIO_Pin_15;
	gpiod15.GPIO_Mode = GPIO_Mode_AF;
	gpiod15.GPIO_OType = GPIO_OType_PP;
	gpiod15.GPIO_Speed = GPIO_Speed_100MHz;
	gpiod15.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&gpiod15);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_TIM4);
	
	
	/////////////////////////////////////
	//TIM
	/////////////////////////////////////
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	TIM_TimeBaseInitTypeDef timebase;
	TIM_TimeBaseStructInit(&timebase);
	timebase.TIM_ClockDivision = TIM_CKD_DIV1;
	timebase.TIM_Prescaler = 84-1;
	timebase.TIM_Period = 15000-1;
	timebase.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4,&timebase);
	
	TIM_OCInitTypeDef oc1def;
	TIM_OCStructInit(&oc1def);
	oc1def.TIM_OCMode = TIM_OCMode_PWM1;
	oc1def.TIM_OutputState = TIM_OutputState_Enable;
	oc1def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc1def.TIM_Pulse = 1500-1;
	TIM_OC1Init(TIM4,&oc1def);
	
	TIM_OCInitTypeDef oc2def;
	TIM_OCStructInit(&oc2def);
	oc2def.TIM_OCMode = TIM_OCMode_PWM1;
	oc2def.TIM_OutputState = TIM_OutputState_Enable;
	oc2def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc2def.TIM_Pulse = 1500-1;
	TIM_OC2Init(TIM4,&oc2def);
	
	TIM_OCInitTypeDef oc3def;
	TIM_OCStructInit(&oc3def);
	oc3def.TIM_OCMode = TIM_OCMode_PWM1;
	oc3def.TIM_OutputState = TIM_OutputState_Enable;
	oc3def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc3def.TIM_Pulse = 1500-1;
	TIM_OC3Init(TIM4,&oc3def);
	
	TIM_OCInitTypeDef oc4def;
	TIM_OCStructInit(&oc4def);
	oc4def.TIM_OCMode = TIM_OCMode_PWM1;
	oc4def.TIM_OutputState = TIM_OutputState_Enable;
	oc4def.TIM_OCPolarity = TIM_OCPolarity_High;
	oc4def.TIM_Pulse = 1500-1;
	TIM_OC4Init(TIM4,&oc4def);
	
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);
}

void ServoTim4::timerStart(){
	TIM_Cmd(TIM4,ENABLE);
}

void ServoTim4::setDuty(int ch,uint32_t duty){
	switch(ch){
	case 1:TIM_SetCompare1(TIM4,duty);break;
	case 2:TIM_SetCompare2(TIM4,duty);break;
	case 3:TIM_SetCompare3(TIM4,duty);break;
	case 4:TIM_SetCompare4(TIM4,duty);break;
	}
}
