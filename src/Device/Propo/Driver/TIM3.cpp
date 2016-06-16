#include <pch.h>
#include "TIM3.h"

PropoTim3::PropoTim3(){
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
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
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
	timebase.TIM_Prescaler = 84-1;
	timebase.TIM_Period = 15000-1;
	timebase.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&timebase);
	
	TIM_ICInitTypeDef ic1def;
	TIM_ICStructInit(&ic1def);
	ic1def.TIM_Channel = TIM_Channel_1;
	ic1def.TIM_ICPolarity = TIM_ICPolarity_Falling;
	ic1def.TIM_ICSelection = TIM_ICSelection_DirectTI;
	ic1def.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	ic1def.TIM_ICFilter = 0x2;
	TIM_ICInit(TIM3,&ic1def);
	TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Enable);
	
	TIM_ICInitTypeDef ic2def;
	TIM_ICStructInit(&ic2def);
	ic2def.TIM_Channel = TIM_Channel_2;
	ic2def.TIM_ICPolarity = TIM_ICPolarity_Falling;
	ic2def.TIM_ICSelection = TIM_ICSelection_DirectTI;
	ic2def.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	ic2def.TIM_ICFilter = 0x2;
	TIM_ICInit(TIM3,&ic2def);
	TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Enable);
	
	TIM_ICInitTypeDef ic3def;
	TIM_ICStructInit(&ic3def);
	ic3def.TIM_Channel = TIM_Channel_3;
	ic3def.TIM_ICPolarity = TIM_ICPolarity_Falling;
	ic3def.TIM_ICSelection = TIM_ICSelection_DirectTI;
	ic3def.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	ic3def.TIM_ICFilter = 0x2;
	TIM_ICInit(TIM3,&ic3def);
	TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Enable);
	
	TIM_ICInitTypeDef ic4def;
	TIM_ICStructInit(&ic4def);
	ic4def.TIM_Channel = TIM_Channel_4;
	ic4def.TIM_ICPolarity = TIM_ICPolarity_Falling;
	ic4def.TIM_ICSelection = TIM_ICSelection_DirectTI;
	ic4def.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	ic4def.TIM_ICFilter = 0x2;
	TIM_ICInit(TIM3,&ic4def);
	TIM_CCxCmd(TIM3,TIM_Channel_4,TIM_CCx_Enable);
	
	TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);
	TIM_SelectInputTrigger(TIM3,TIM_TS_TI1F_ED);
}

void PropoTim3::timerStart(){
	TIM_Cmd(TIM3,ENABLE);
}
int PropoTim3::getUsTime(int ch){
	if(ch == 0){
		return TIM_GetCapture1(TIM3);
	}else if(ch == 1){
		return TIM_GetCapture2(TIM3);
	}else if(ch == 2){
		return TIM_GetCapture3(TIM3);
	}else if(ch == 3){
		return TIM_GetCapture4(TIM3);
	}else{
		return 0;
	}
}


