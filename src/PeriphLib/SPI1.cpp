#include "stdio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "SPI1.h"

#include "task.h"
#include "queue.h"


SPI1Class::SPI1Class(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef pa4def;

	GPIO_StructInit(&pa4def);
	pa4def.GPIO_Pin = GPIO_Pin_4;
	pa4def.GPIO_Mode = GPIO_Mode_AF;
	pa4def.GPIO_OType = GPIO_OType_PP;
	pa4def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pa4def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&pa4def);

	GPIO_InitTypeDef pa5def;

	GPIO_StructInit(&pa5def);
	pa5def.GPIO_Pin = GPIO_Pin_5;
	pa5def.GPIO_Mode = GPIO_Mode_AF;
	pa5def.GPIO_OType = GPIO_OType_PP;
	pa5def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pa5def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&pa5def);

	GPIO_InitTypeDef pa6def;

	GPIO_StructInit(&pa6def);
	pa6def.GPIO_Pin = GPIO_Pin_6;
	pa6def.GPIO_Mode = GPIO_Mode_AF;
	pa6def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pa6def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&pa6def);

	GPIO_InitTypeDef pa7def;

	GPIO_StructInit(&pa7def);
	pa7def.GPIO_Pin = GPIO_Pin_7;
	pa7def.GPIO_Mode = GPIO_Mode_AF;
	pa7def.GPIO_OType = GPIO_OType_PP;
	pa7def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pa7def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&pa7def);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);


	SPI_InitTypeDef spi1;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	spi1.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	spi1.SPI_CPHA = SPI_CPHA_2Edge;
	spi1.SPI_CPOL = SPI_CPOL_High;
	spi1.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi1.SPI_DataSize = SPI_DataSize_16b;
	spi1.SPI_FirstBit = SPI_FirstBit_MSB;
	spi1.SPI_Mode = SPI_Mode_Master;
	spi1.SPI_NSS = SPI_NSS_Hard;

	SPI_Init(SPI1,&spi1);

	SPI_SSOutputCmd(SPI1,ENABLE);
	SPI_I2S_DMACmd(SPI1,SPI_DMAReq_Rx|SPI_DMAReq_Tx,ENABLE);

	SPI_Cmd(SPI1,ENABLE);


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);

	DMA_InitTypeDef dma2_2;
	DMA_StructInit(&dma2_2);
	dma2_2.DMA_PeripheralBaseAddr = (uint32_t)&(SPI1->DR);
	dma2_2.DMA_Memory0BaseAddr = (uint32_t)m_rxBuf;
	dma2_2.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma2_2.DMA_BufferSize = 4;
	dma2_2.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma2_2.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma2_2.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma2_2.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma2_2.DMA_Mode = DMA_Mode_Circular;
	dma2_2.DMA_Priority = DMA_Priority_Medium;

	dma2_2.DMA_Channel = DMA_Channel_3;
	DMA_Init(DMA2_Stream2,&dma2_2);

	DMA_Cmd(DMA2_Stream2,ENABLE);

	DMA_InitTypeDef dma2_3;
	DMA_StructInit(&dma2_3);
	dma2_3.DMA_PeripheralBaseAddr = (uint32_t)&(SPI1->DR);
	dma2_3.DMA_Memory0BaseAddr = (uint32_t)m_txBuf;
	dma2_3.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma2_3.DMA_BufferSize = SPI_BUFFERSIZE;
	dma2_3.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma2_3.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma2_3.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma2_3.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma2_3.DMA_Mode = DMA_Mode_Normal;
	dma2_3.DMA_Priority = DMA_Priority_Medium;

	dma2_3.DMA_Channel = DMA_Channel_3;
	DMA_Init(DMA2_Stream3,&dma2_3);


	//SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

	GPIO_InitTypeDef pa0def;

	GPIO_StructInit(&pa0def);
	pa0def.GPIO_Pin = GPIO_Pin_0;
	pa0def.GPIO_Mode = GPIO_Mode_IN;
	pa0def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pa0def.GPIO_Speed = GPIO_Speed_100MHz;
	//GPIO_Init(GPIOA,&pa0def);
	GPIO_Init(GPIOC,&pa0def);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);

	EXTI_InitTypeDef exti0def;
	EXTI_StructInit(&exti0def);
	exti0def.EXTI_Line = EXTI_Line0;
	exti0def.EXTI_Mode = EXTI_Mode_Interrupt;
	exti0def.EXTI_Trigger = EXTI_Trigger_Falling;
	exti0def.EXTI_LineCmd = ENABLE;

	EXTI_Init(&exti0def);


	NVIC_InitTypeDef nvicdef;
	nvicdef.NVIC_IRQChannel = EXTI0_IRQn;
	nvicdef.NVIC_IRQChannelPreemptionPriority = 0x00;
	nvicdef.NVIC_IRQChannelSubPriority = 0x00;
	nvicdef.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicdef);

	NVIC_SetPriority(EXTI0_IRQn,0xFF);
}

int SPI1Class::ReadWrite(unsigned char* outReadData,unsigned char* writeData,int byteRwLength)
{
	m_txBuf[0] = writeData[0];
	m_txBuf[1] = writeData[1];
	m_txBuf[2] = writeData[2];
	m_txBuf[3] = writeData[3];

	DMA_Cmd(DMA2_Stream3,DISABLE);
	DMA_SetCurrDataCounter(DMA2_Stream3,4);
	DMA_ClearFlag(DMA2_Stream3,DMA_FLAG_TCIF3);
	DMA_Cmd(DMA2_Stream3,ENABLE);

	return 0;
}
