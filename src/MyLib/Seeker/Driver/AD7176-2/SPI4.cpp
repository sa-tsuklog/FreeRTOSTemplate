/*
 * SPI4.cpp
 *
 *  Created on: 2014/03/14
 *      Author: lark-mp
 */

#include <pch.h>
#include "SPI4.h"
#include "MyLib/Util/Util.h"
//not checked

SPI4Class::SPI4Class(){
	m_rwSem = xSemaphoreCreateBinary();
	m_dataReadySem = xSemaphoreCreateBinary();
	if(m_rwSem == NULL || m_dataReadySem == NULL){
		Util::GetInstance()->myFprintf(0,stdout,"malloc error at SPI4 initialization\r\n");
		while(1){}
	}

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);

	GPIO_InitTypeDef pe2def;
	GPIO_StructInit(&pe2def);
	pe2def.GPIO_Pin = GPIO_Pin_2;
	pe2def.GPIO_Mode = GPIO_Mode_AF;
	pe2def.GPIO_OType = GPIO_OType_PP;
	pe2def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pe2def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&pe2def);
	
	GPIO_InitTypeDef pe4def;
	GPIO_StructInit(&pe4def);
	pe4def.GPIO_Pin = GPIO_Pin_4;
	pe4def.GPIO_Mode = GPIO_Mode_OUT;
	pe4def.GPIO_OType = GPIO_OType_PP;
	pe4def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pe4def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&pe4def);

	GPIO_InitTypeDef pe5def;
	GPIO_StructInit(&pe5def);
	pe5def.GPIO_Pin = GPIO_Pin_5;
	pe5def.GPIO_Mode = GPIO_Mode_AF;
	pe5def.GPIO_OType = GPIO_OType_PP;
	pe5def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pe5def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&pe5def);

	GPIO_InitTypeDef pe6def;
	GPIO_StructInit(&pe6def);
	pe6def.GPIO_Pin = GPIO_Pin_6;
	pe6def.GPIO_Mode = GPIO_Mode_AF;
	pe6def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pe6def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&pe6def);	

	GPIO_SetBits(GPIOE,GPIO_Pin_4);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource2,GPIO_AF_SPI4);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_SPI4);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource6,GPIO_AF_SPI4);
	
	

	///////////////////////////
	//SPI Setting
	///////////////////////////
	SPI_InitTypeDef spi4;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4,ENABLE);
	spi4.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	spi4.SPI_CPHA = SPI_CPHA_2Edge;
	spi4.SPI_CPOL = SPI_CPOL_High;
	spi4.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi4.SPI_DataSize = SPI_DataSize_8b;
	spi4.SPI_FirstBit = SPI_FirstBit_MSB;
	spi4.SPI_Mode = SPI_Mode_Master;
	spi4.SPI_NSS = SPI_NSS_Hard;

	SPI_Init(SPI4,&spi4);

	SPI_SSOutputCmd(SPI4,ENABLE);
	SPI_NSSInternalSoftwareConfig(SPI4,SPI_NSSInternalSoft_Set);
	SPI_I2S_DMACmd(SPI4,SPI_DMAReq_Tx|SPI_DMAReq_Rx,ENABLE);
	SPI_Cmd(SPI4,ENABLE);

	

	DMA_InitTypeDef dma2_0;
	DMA_StructInit(&dma2_0);
	dma2_0.DMA_PeripheralBaseAddr = (uint32_t)&(SPI4->DR);
	dma2_0.DMA_Memory0BaseAddr = (uint32_t)m_rxBuf;
	dma2_0.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma2_0.DMA_BufferSize = SPI_BUFFERSIZE;
	dma2_0.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma2_0.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma2_0.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma2_0.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma2_0.DMA_Mode = DMA_Mode_Normal;
	dma2_0.DMA_Priority = DMA_Priority_Medium;
	dma2_0.DMA_Channel = DMA_Channel_4;
	DMA_Init(DMA2_Stream0,&dma2_0);
	DMA_Cmd(DMA2_Stream0,ENABLE);

	DMA_InitTypeDef dma2_1;
	DMA_StructInit(&dma2_1);
	dma2_1.DMA_PeripheralBaseAddr = (uint32_t)&(SPI4->DR);
	dma2_1.DMA_Memory0BaseAddr = (uint32_t)m_txBuf;
	dma2_1.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma2_1.DMA_BufferSize = SPI_BUFFERSIZE;
	dma2_1.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma2_1.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma2_1.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma2_1.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma2_1.DMA_Mode = DMA_Mode_Normal;
	dma2_1.DMA_Priority = DMA_Priority_Medium;

	dma2_1.DMA_Channel = DMA_Channel_4;
	DMA_Init(DMA2_Stream1,&dma2_1);
	DMA_Cmd(DMA2_Stream1,DISABLE);
	
	
	DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource5);


	EXTI_InitTypeDef exti5def;
	EXTI_StructInit(&exti5def);
	exti5def.EXTI_Line = EXTI_Line5;
	exti5def.EXTI_Mode = EXTI_Mode_Interrupt;
	exti5def.EXTI_Trigger = EXTI_Trigger_Falling;
	exti5def.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti5def);
	disableEXTI5();
	
	NVIC_InitTypeDef exti_nvicdef;
	exti_nvicdef.NVIC_IRQChannel  = EXTI9_5_IRQn;
	exti_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	exti_nvicdef.NVIC_IRQChannelSubPriority = 0xFF;
	exti_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&exti_nvicdef);
	NVIC_SetPriority(EXTI9_5_IRQn,0xFF);

	NVIC_InitTypeDef dma_nvicdef;
	dma_nvicdef.NVIC_IRQChannel  = DMA2_Stream1_IRQn;
	dma_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	dma_nvicdef.NVIC_IRQChannelSubPriority = 0xFF;
	dma_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&dma_nvicdef);
	
	NVIC_SetPriority(DMA2_Stream1_IRQn,0xFF);
}

int SPI4Class::ReadWrite(unsigned char* outReadData,unsigned char* writeData,int byteRwLength){
	GPIO_ResetBits(GPIOE,GPIO_Pin_4);

	DMA_Cmd(DMA2_Stream0,DISABLE);
	DMA_Cmd(DMA2_Stream1,DISABLE);

	for(int i=0;i<byteRwLength && i < SPI_BUFFERSIZE;i++){
		m_txBuf[i] = writeData[i];
	}
	DMA_SetCurrDataCounter(DMA2_Stream0,byteRwLength);
	DMA_SetCurrDataCounter(DMA2_Stream1,byteRwLength);
	DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);
	DMA_ClearITPendingBit(DMA2_Stream1,DMA_IT_TCIF1);

	DMA_Cmd(DMA2_Stream0,ENABLE);
	DMA_Cmd(DMA2_Stream1,ENABLE);
	

	xSemaphoreTake(m_rwSem,portMAX_DELAY);

	GPIO_SetBits(GPIOE,GPIO_Pin_4);

	for(int i=0;i<byteRwLength && i < SPI_BUFFERSIZE;i++){
		outReadData[i] = m_rxBuf[i];
	}

	if(byteRwLength < SPI_BUFFERSIZE){
		return byteRwLength;
	}else{
		return SPI_BUFFERSIZE;
	}
}

void SPI4Class::enableEXTI5(){
	EXTI->IMR |= EXTI_Line5;
}

void SPI4Class::disableEXTI5(){
	EXTI->IMR &= ~EXTI_Line5;
}

void SPI4Class::waitForDataReady(){
	EXTI_ClearITPendingBit(EXTI_Line5);
	enableEXTI5();
	GPIO_ResetBits(GPIOE,GPIO_Pin_4);
	if(m_dataReadySem!=NULL){
		xSemaphoreTake(m_dataReadySem,portMAX_DELAY);
	}
	disableEXTI5();
}

void SPI4Class::myEXTI5_IRQHandler(){
	if(m_dataReadySem!=NULL){
		BaseType_t isHigherPriorityWoken;
		xSemaphoreGiveFromISR(m_dataReadySem,&isHigherPriorityWoken);
		portEND_SWITCHING_ISR(isHigherPriorityWoken);
	}
	
}

void SPI4Class::myDMA2_Stream1_IRQHandler(){
	if(DMA_GetITStatus(DMA2_Stream1,DMA_IT_TCIF1)!=RESET){
		BaseType_t isHigherPriorityWoken;
		DMA_ClearITPendingBit(DMA2_Stream1,DMA_IT_TCIF1);
		xSemaphoreGiveFromISR(m_rwSem,&isHigherPriorityWoken);
		portEND_SWITCHING_ISR(isHigherPriorityWoken);
	}
}
