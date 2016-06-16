/*
 * DAC.cpp
 *
 *  Created on: 2014/04/28
 *      Author: sa
 */
#include <pch.h>
#include "DAC.h"

DACClass::DACClass(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	/////////////////////////////////////
	//GPIO
	/////////////////////////////////////
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef gpioa4_5;
	GPIO_StructInit(&gpioa4_5);
	gpioa4_5.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	gpioa4_5.GPIO_Mode = GPIO_Mode_AN;
	gpioa4_5.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&gpioa4_5);
	
	/////////////////////////////////////
	//DAC
	/////////////////////////////////////
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
	DAC_InitTypeDef dac;
	DAC_StructInit(&dac);
	dac.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1,&dac);
	DAC_Init(DAC_Channel_2,&dac);
	DAC_Cmd(DAC_Channel_1,ENABLE);
	DAC_Cmd(DAC_Channel_2,ENABLE);
}

void DACClass::setValue(int ch,int value){
	if(ch == 1){
		DAC_SetChannel1Data(DAC_Align_12b_R,value);
	}else{
		DAC_SetChannel2Data(DAC_Align_12b_R,value);
	}
}

void DACClass::setValue(int ch,float value){
	setValue(ch,(int)(value*4096));
}

void DACClass::setValues(int value1,int value2){
	DAC_SetDualChannelData(DAC_Align_12b_R,value2,value1);
}
void DACClass::setValues(float value1,float value2){
	setValues((int)(4096*value1),(int)(4096*value2));
}
