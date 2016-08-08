/*
 * LedGpio.cpp
 *
 *  Created on: 2016/07/17
 *      Author: sa
 */

#include "stm32f4xx.h"
#include "LedGpio.h"

LedGpio::LedGpio(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef gpiob8;
	GPIO_StructInit(&gpiob8);
	gpiob8.GPIO_Pin = GPIO_Pin_8;
	gpiob8.GPIO_Mode = GPIO_Mode_OUT;
	gpiob8.GPIO_OType = GPIO_OType_PP;
	gpiob8.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&gpiob8);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
}

void LedGpio::set(uint32_t on){
	if(on){
		GPIO_SetBits(GPIOB,GPIO_Pin_8);
	}else{
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	}
}
