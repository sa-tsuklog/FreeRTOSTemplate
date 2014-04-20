/*
 * TIM2.cpp
 *
 *  Created on: 2014/04/20
 *      Author: sa
 */
#include "stdio.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "TIM2.h"

//for supersonic height
TIM2Class::TIM2Class(){
	/////////////////////////////////////
	//GPIO
	/////////////////////////////////////
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef gpioa0;
	GPIO_StructInit(&gpioa0);
	gpioa0.GPIO_Pin = GPIO_Pin_0;
	gpioa0.GPIO_Mode = GPIO_Mode_AF;
	gpioa0.GPIO_OType = GPIO_OType_PP;
	gpioa0.GPIO_Speed = GPIO_Speed_100MHz;
	gpioa0.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&gpioa0);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2);
	
	/////////////////////////////////////
	//TIM
	/////////////////////////////////////
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_TimeBaseInitTypeDef timebase;
	TIM_TimeBaseStructInit(&timebase);
	timebase.TIM_ClockDivision = TIM_CKD_DIV1;
	timebase.TIM_Prescaler = 84-1;
	timebase.TIM_Period = 65536-1;
	timebase.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&timebase);
	
	//TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);
	
	TIM_SelectInputTrigger(TIM2,TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM2,TIM_SlaveMode_Reset);
	
	
	TIM_ICInitTypeDef ic1def;
	TIM_ICStructInit(&ic1def);
	ic1def.TIM_Channel = TIM_Channel_1;
	ic1def.TIM_ICFilter = 0x0011;//fsampling = fck_int, N = 8
	ic1def.TIM_ICPolarity = TIM_ICPolarity_Rising;
	ic1def.TIM_ICPrescaler = 0;
	ic1def.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM2,&ic1def);
	
	TIM_ICInitTypeDef ic2def;
	TIM_ICStructInit(&ic2def);
	ic2def.TIM_Channel = TIM_Channel_2;
	ic2def.TIM_ICFilter = 0x0011;//fsampling = fck_int, N = 8
	ic2def.TIM_ICPolarity = TIM_ICPolarity_Falling;
	ic2def.TIM_ICPrescaler = 0;
	ic2def.TIM_ICSelection = TIM_ICSelection_IndirectTI;
	TIM_ICInit(TIM2,&ic2def);
	
	NVIC_InitTypeDef nvicdef;
	nvicdef.NVIC_IRQChannel = TIM2_IRQn;
	nvicdef.NVIC_IRQChannelCmd = ENABLE;
	nvicdef.NVIC_IRQChannelPreemptionPriority = 0x00;
	nvicdef.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_Init(&nvicdef);
}

void TIM2Class::timerStart(){
	TIM_Cmd(TIM2,ENABLE);
}
int TIM2Class::getUsCycle(){
	return TIM_GetCapture2(TIM2);
}
int TIM2Class::getUsDuty(){
	return TIM_GetCapture1(TIM2);
}

void prvTIM2TestTask(void* pvParameters){
	TIM2Class::GetInstance()->timerStart();
	while(1){
		unsigned short s2= TIM_GetCounter(TIM2);
		//printf("ch2:%d,%d,%d\n\r",TIM2Class::GetInstance()->getUsCycle(),
		//						TIM2Class::GetInstance()->getUsDuty(),s2);
		vTaskDelay(100);
	}
}

