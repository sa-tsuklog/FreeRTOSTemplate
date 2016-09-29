/*
 * ImuSpi2.cpp
 *
 *  Created on: 2016/09/28
 *      Author: sa
 */

#include <pch.h>
#include "ImuSpi2.h"
#include "stdio.h"

ImuSpi2::ImuSpi2(){
}

void ImuSpi2::init(){
	rwSem = xSemaphoreCreateBinary();
	dataReadySem = xSemaphoreCreateBinary();
	if(rwSem == NULL || dataReadySem == NULL){
		fprintf(stdout,"malloc error at SPI2 initialization\r\n");
		while(1){}
	}
	
	//////////////////////
	//GPIO Setting
	//////////////////////
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	GPIO_InitTypeDef pb13def;
	GPIO_StructInit(&pb13def);
	pb13def.GPIO_Pin = GPIO_Pin_13;
	pb13def.GPIO_Mode = GPIO_Mode_AF;
	pb13def.GPIO_OType = GPIO_OType_PP;
	pb13def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pb13def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&pb13def);

	GPIO_InitTypeDef pb14def;
	GPIO_StructInit(&pb14def);
	pb14def.GPIO_Pin = GPIO_Pin_14;
	pb14def.GPIO_Mode = GPIO_Mode_AF;
	pb14def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pb14def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&pb14def);

	GPIO_InitTypeDef pb15def;
	GPIO_StructInit(&pb15def);
	pb15def.GPIO_Pin = GPIO_Pin_15;
	pb15def.GPIO_Mode = GPIO_Mode_AF;
	pb15def.GPIO_OType = GPIO_OType_PP;
	pb15def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pb15def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&pb15def);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	
	//NSS port setting	
	//ICM-20601
	GPIO_InitTypeDef pa12def;
	GPIO_StructInit(&pa12def);
	pa12def.GPIO_Pin = GPIO_Pin_12;
	pa12def.GPIO_Mode = GPIO_Mode_OUT;
	pa12def.GPIO_OType = GPIO_OType_PP;
	pa12def.GPIO_Speed = GPIO_Speed_25MHz;
	pa12def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&pa12def);
	
	//AK8963
	GPIO_InitTypeDef pe15def;
	GPIO_StructInit(&pe15def);
	pe15def.GPIO_Pin = GPIO_Pin_15;
	pe15def.GPIO_Mode = GPIO_Mode_OUT;
	pe15def.GPIO_OType = GPIO_OType_PP;
	pe15def.GPIO_Speed = GPIO_Speed_25MHz;
	pe15def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&pe15def);
	
	//LPS25HB
	GPIO_InitTypeDef pa10def;
	GPIO_StructInit(&pa10def);
	pa10def.GPIO_Pin = GPIO_Pin_10;
	pa10def.GPIO_Mode = GPIO_Mode_OUT;
	pa10def.GPIO_OType = GPIO_OType_PP;
	pa10def.GPIO_Speed = GPIO_Speed_25MHz;
	pa10def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&pa10def);
	
	csDeassert(CH_ICM20601);
	csDeassert(CH_LPS25HB);
	csDeassert(CH_AK8963);
	
	
	//ICM-20601 misc
	//int
	GPIO_InitTypeDef pa11def;
	GPIO_StructInit(&pa11def);
	pa11def.GPIO_Pin = GPIO_Pin_11;
	pa11def.GPIO_Mode = GPIO_Mode_IN;
	pa11def.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA,&pa11def);
	
	//AK8963 misc
	//rstn
	GPIO_InitTypeDef pb11def;
	GPIO_StructInit(&pb11def);
	pb11def.GPIO_Pin = GPIO_Pin_11;
	pb11def.GPIO_Mode = GPIO_Mode_OUT;
	pb11def.GPIO_OType = GPIO_OType_PP;
	pb11def.GPIO_Speed = GPIO_Speed_25MHz;
	pb11def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&pb11def);
	GPIO_SetBits(GPIOB,GPIO_Pin_11);
	
	//trig
	GPIO_InitTypeDef pb12def;
	GPIO_StructInit(&pb12def);
	pb12def.GPIO_Pin = GPIO_Pin_12;
	pb12def.GPIO_Mode = GPIO_Mode_IN;
	pb12def.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB,&pb12def);
	
	//drdy
	GPIO_InitTypeDef pb10def;
	GPIO_StructInit(&pb10def);
	pb10def.GPIO_Pin = GPIO_Pin_10;
	pb10def.GPIO_Mode = GPIO_Mode_IN;
	pb10def.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB,&pb10def);
	
	
	//LPS25HB misc
	GPIO_InitTypeDef pa9def;
	GPIO_StructInit(&pa9def);
	pa9def.GPIO_Pin = GPIO_Pin_9;
	pa9def.GPIO_Mode = GPIO_Mode_IN;
	pa9def.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA,&pa9def);
	
	
	///////////////////////////
	//SPI Setting
	///////////////////////////
	SPI_InitTypeDef SPI2def;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	SPI2def.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI2def.SPI_CPHA = SPI_CPHA_2Edge;
	SPI2def.SPI_CPOL = SPI_CPOL_High;
	SPI2def.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI2def.SPI_DataSize = SPI_DataSize_16b;
	SPI2def.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI2def.SPI_Mode = SPI_Mode_Master;
	SPI2def.SPI_NSS = SPI_NSS_Hard;
	
	SPI_Init(SPI2,&SPI2def);
	SPI_SSOutputCmd(SPI2,ENABLE);
	//SPI_I2S_DMACmd(SPI2,SPI_DMAReq_Rx|SPI_DMAReq_Tx,ENABLE);
	SPI_I2S_DMACmd(SPI2,SPI_DMAReq_Rx,ENABLE);
	SPI_Cmd(SPI2,ENABLE);
	
	/////////////////////////////
	//DMA Setting
	/////////////////////////////
	//SPI2RX
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	DMA_InitTypeDef dma1_3;
	DMA_StructInit(&dma1_3);
	dma1_3.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	dma1_3.DMA_Memory0BaseAddr = 0;
	dma1_3.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma1_3.DMA_BufferSize = 2;
	dma1_3.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_3.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_3.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma1_3.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma1_3.DMA_Mode = DMA_Mode_Circular;
	dma1_3.DMA_Priority = DMA_Priority_Medium;

	dma1_3.DMA_Channel = DMA_Channel_0;
	DMA_Init(DMA1_Stream3,&dma1_3);
	DMA_Cmd(DMA1_Stream3,ENABLE);
	
	//TIM8_CH1
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	DMA_InitTypeDef dma2_2;
	DMA_StructInit(&dma2_2);
	dma2_2.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	dma2_2.DMA_Memory0BaseAddr = 0;
	dma2_2.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma2_2.DMA_BufferSize = 2;
	dma2_2.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma2_2.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma2_2.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma2_2.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma2_2.DMA_Mode = DMA_Mode_Circular;
	dma2_2.DMA_Priority = DMA_Priority_Medium;

	dma2_2.DMA_Channel = DMA_Channel_0;
	DMA_Init(DMA2_Stream2,&dma2_2);
	DMA_Cmd(DMA2_Stream2,ENABLE);
	
	
	////////////////////////////
	//Interrupt
	/////////////////////////////
	//DMA
	NVIC_InitTypeDef dma_nvicdef;
	dma_nvicdef.NVIC_IRQChannel  = DMA2_Stream2_IRQn;
	dma_nvicdef.NVIC_IRQChannelPreemptionPriority = 0x00;
	dma_nvicdef.NVIC_IRQChannelSubPriority = 0x00;
	dma_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&dma_nvicdef);
	//NVIC_SetPriority(DMA2_Stream6_IRQn,0x08);

	//Trigger
	EXTI_InitTypeDef exti11def;
	EXTI_StructInit(&exti11def);
	exti11def.EXTI_Line = EXTI_Line11;
	exti11def.EXTI_Mode = EXTI_Mode_Interrupt;
	exti11def.EXTI_Trigger = EXTI_Trigger_Rising;
	exti11def.EXTI_LineCmd =ENABLE;
	EXTI_Init(&exti11def);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource11);
	
	
	NVIC_InitTypeDef exti11itdef;
	exti11itdef.NVIC_IRQChannel = EXTI9_5_IRQn;
	exti11itdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	exti11itdef.NVIC_IRQChannelSubPriority = 0xFF;
	exti11itdef.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&exti11itdef);
	NVIC_SetPriority(EXTI15_10_IRQn,0xFF);
	
	vTaskDelay(100);
}

void ImuSpi2::csAssert(uint8_t ch){
	if(ch == CH_AK8963){
		GPIO_ResetBits(GPIOE,GPIO_Pin_15);
	}else if(ch == CH_ICM20601){
		GPIO_ResetBits(GPIOA,GPIO_Pin_12);
	}else if(ch == CH_LPS25HB){
		GPIO_ResetBits(GPIOA,GPIO_Pin_10);
	}
}
void ImuSpi2::csDeassert(uint8_t ch){
	if(ch == CH_AK8963){
		GPIO_SetBits(GPIOE,GPIO_Pin_15);
	}else if(ch == CH_ICM20601){
		GPIO_SetBits(GPIOA,GPIO_Pin_12);
	}else if(ch == CH_LPS25HB){
		GPIO_SetBits(GPIOA,GPIO_Pin_10);
	}
}

void ImuSpi2::waitNewData(){
	xSemaphoreTake(dataReadySem,0);
	xSemaphoreTake(dataReadySem,portMAX_DELAY);
}

int ReadWrite(uint8_t ch,unsigned char* outReadData,unsigned char* writeData,int byteRwLength){
	
}

void ImuSpi2::myEXTI11_IRQHandler(void)
{
	portBASE_TYPE isSWitchRequired;
	xSemaphoreGiveFromISR(dataReadySem,&isSWitchRequired);
	portEND_SWITCHING_ISR(isSWitchRequired);
}
