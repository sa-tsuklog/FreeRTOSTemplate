/*
 * DMA_TIM8.cpp
 *
 *  Created on: 2015/02/25
 *      Author: sa
 */

#include "stm32f4xx.h"
#include "DAC_TIM8.h"
#include "stdlib.h"
#include "math.h"

DAC_TIM6Class::DAC_TIM6Class(){
	waveBuffer = malloc(sizeof(uint16_t) * WAVE_MEMORY_LENGTH);
	if(waveBuffer==NULL){
		while(1){}
	}
	for(int i=0;i<WAVE_MEMORY_LENGTH;i++){
		waveBuffer[i] = 2048+1024*sinf(2*M_PI*i/WAVE_MEMORY_LENGTH);
	}
	
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	/////////////////////////////////////
	//GPIO
	/////////////////////////////////////
	GPIO_InitTypeDef gpioa4_5;
	GPIO_StructInit(&gpioa4_5);
	gpioa4_5.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	gpioa4_5.GPIO_Mode = GPIO_Mode_AN;
	gpioa4_5.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&gpioa4_5);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
	/////////////////////////////////////
	//DAC
	/////////////////////////////////////
	DAC_InitTypeDef dac1;
	DAC_StructInit(&dac1);
	dac1.DAC_LFSRUnmask_TriangleAmplitude =DAC_LFSRUnmask_Bits11_0;
	dac1.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	dac1.DAC_Trigger = DAC_Trigger_T6_TRGO;
	dac1.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_Init(DAC_Channel_1,&dac1);
	
	DAC_SetDualChannelData(DAC_Align_12b_R,0,0);
	DAC_Cmd(DAC_Channel_1,ENABLE);
	
	DAC_DMACmd(DAC_Channel_1,ENABLE);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	TIM_TimeBaseInitTypeDef timebase;
	timebase.TIM_ClockDivision = TIM_CKD_DIV1;
	timebase.TIM_CounterMode = TIM_CounterMode_Up;
	timebase.TIM_Prescaler = 168-1;
	timebase.TIM_Period = 1;
	TIM_TimeBaseInit(TIM6,&timebase);
	
	TIM6->CR2 = 0x20;	//MasterModeSelection
	
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	
	DMA_InitTypeDef dma1_5;
	DMA_StructInit(&dma1_5);
	dma1_5.DMA_PeripheralBaseAddr = (uint32_t)&(DAC->DHR12R1);
	dma1_5.DMA_Memory0BaseAddr = (uint32_t)waveBuffer;
	dma1_5.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma1_5.DMA_BufferSize = WAVE_MEMORY_LENGTH;
	dma1_5.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_5.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_5.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma1_5.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma1_5.DMA_Mode = DMA_Mode_Circular;
	dma1_5.DMA_Priority = DMA_Priority_VeryHigh;
	dma1_5.DMA_Channel = DMA_Channel_7;
	DMA_Init(DMA1_Stream5,&dma1_5);
	
	DMA_Cmd(DMA1_Stream5,ENABLE);
	
	
	TIM_Cmd(TIM6,ENABLE);
}

void DAC_TIM6Class::setData(int i){
	//DAC_SetChannel1Data(DAC_Align_12b_R,i);
	DAC->DHR12R1 = i;
}
