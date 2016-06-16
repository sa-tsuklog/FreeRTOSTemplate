/*
 * TIM5.cpp
 *
 *  Created on: 2014/04/20
 *      Author: sa
 */

/*
 * TIM5.cpp
 *
 *  Created on: 2014/04/20
 *      Author: sa
 */
#include <pch.h>
#include "TIM5.h"

//for propo input
TIM5Class::TIM5Class(){
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
	timebase.TIM_Period = 65536-1;
	timebase.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5,&timebase);
	
	TIM_ITConfig(TIM5,TIM_IT_CC2,ENABLE);
	
	TIM_SelectInputTrigger(TIM5,TIM_TS_TI2FP2);
	TIM_SelectSlaveMode(TIM5,TIM_SlaveMode_Reset);
	
	
	TIM_ICInitTypeDef ic1def;
	TIM_ICStructInit(&ic1def);
	ic1def.TIM_Channel = TIM_Channel_1;
	ic1def.TIM_ICFilter = 0x0011;//fsampling = fck_int, N = 8
	ic1def.TIM_ICPolarity = TIM_ICPolarity_Falling;
	ic1def.TIM_ICPrescaler = 0;
	ic1def.TIM_ICSelection = TIM_ICSelection_IndirectTI;
	TIM_ICInit(TIM5,&ic1def);
	
	TIM_ICInitTypeDef ic2def;
	TIM_ICStructInit(&ic2def);
	ic2def.TIM_Channel = TIM_Channel_2;
	ic2def.TIM_ICFilter = 0x0011;//fsampling = fck_int, N = 8
	ic2def.TIM_ICPolarity = TIM_ICPolarity_Rising;
	ic2def.TIM_ICPrescaler = 0;
	ic2def.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM5,&ic2def);
	
	NVIC_InitTypeDef nvicdef;
	nvicdef.NVIC_IRQChannel = TIM5_IRQn;
	nvicdef.NVIC_IRQChannelCmd = ENABLE;
	nvicdef.NVIC_IRQChannelPreemptionPriority = 0x00;
	nvicdef.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_Init(&nvicdef);
}

void TIM5Class::timerStart(){
	TIM_Cmd(TIM5,ENABLE);
}

void prvTIM5TestTask(void* pvParameters){
	TIM5Class::GetInstance()->timerStart();
	while(1){
		unsigned short s = TIM_GetCapture1(TIM5);
		unsigned short s1= TIM_GetCapture2(TIM5);
		unsigned short s2= TIM_GetCounter(TIM5);
		//printf("ch5: %d,%d,%d\r\n",s,s1,s2);
		vTaskDelay(100);
	}
}
