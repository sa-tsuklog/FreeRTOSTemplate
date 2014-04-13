#include "stdio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "SPI1_TIM1.h"

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

	/*
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
	 */

	
	//NSS port setting
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	GPIO_InitTypeDef pe14def;
	GPIO_StructInit(&pe14def);
	pe14def.GPIO_Pin = GPIO_Pin_14;
	pe14def.GPIO_Mode = GPIO_Mode_AF;
	pe14def.GPIO_OType = GPIO_OType_PP;
	pe14def.GPIO_Speed = GPIO_Speed_100MHz;
	pe14def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&pe14def);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_TIM1);
	
	//timebase
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	TIM_TimeBaseInitTypeDef tim1;
	TIM_TimeBaseStructInit(&tim1);
	tim1.TIM_ClockDivision = TIM_CKD_DIV2;
	tim1.TIM_Prescaler = 0;
	tim1.TIM_Period = 1450-1;
	tim1.TIM_CounterMode = TIM_CounterMode_Up;
	tim1.TIM_RepetitionCounter = 3;
	TIM_TimeBaseInit(TIM1,&tim1);
	TIM_SelectOnePulseMode(TIM1,TIM_OPMode_Single);
	
	//NSS generation
	TIM_OCInitTypeDef oc4;
	TIM_OCStructInit(&oc4);
	oc4.TIM_OCMode = TIM_OCMode_PWM1;
	oc4.TIM_OCPolarity = TIM_OCPolarity_High;
	oc4.TIM_OutputState = TIM_OutputState_Enable;
	oc4.TIM_Pulse = 350-1;
	TIM_OC4Init(TIM1,&oc4);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
	//DMA start
	TIM_OCInitTypeDef oc1;
	TIM_OCStructInit(&oc1);
	oc1.TIM_Pulse = 350;
	TIM_OC1Init(TIM1,&oc1);
	
	//Update Interrupt
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	
	
	TIM_DMACmd(TIM1,TIM_DMA_CC1,ENABLE);
	DMA_InitTypeDef dma2_1;
	DMA_StructInit(&dma2_1);
	dma2_1.DMA_PeripheralBaseAddr = (uint32_t)&(SPI1->DR);
	dma2_1.DMA_Memory0BaseAddr = (uint32_t)m_txBuf;
	dma2_1.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma2_1.DMA_BufferSize = 4;
	dma2_1.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma2_1.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma2_1.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma2_1.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma2_1.DMA_Mode = DMA_Mode_Circular;
	dma2_1.DMA_Priority = DMA_Priority_Medium;

	dma2_1.DMA_Channel = DMA_Channel_6;
	DMA_Init(DMA2_Stream1,&dma2_1);
	DMA_Cmd(DMA2_Stream1,ENABLE);
	
	NVIC_InitTypeDef dma_nvicdef;
	dma_nvicdef.NVIC_IRQChannel  = DMA2_Stream1_IRQn;
	dma_nvicdef.NVIC_IRQChannelPreemptionPriority = 0x00;
	dma_nvicdef.NVIC_IRQChannelSubPriority = 0x00;
	dma_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&dma_nvicdef);
	NVIC_SetPriority(DMA1_Stream3_IRQn,0x08);
	
	
	
	//SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

	GPIO_InitTypeDef pa0def;

	GPIO_StructInit(&pa0def);
	pa0def.GPIO_Pin = GPIO_Pin_0;
	pa0def.GPIO_Mode = GPIO_Mode_IN;
	pa0def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pa0def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&pa0def);
	//GPIO_Init(GPIOC,&pa0def);

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
	
	NVIC_InitTypeDef timitdef;
	timitdef.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	timitdef.NVIC_IRQChannelPreemptionPriority = 0x00;
	timitdef.NVIC_IRQChannelSubPriority = 0x00;
	timitdef.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&timitdef);

	NVIC_SetPriority(EXTI0_IRQn,0xFF);
	NVIC_SetPriority(TIM1_UP_TIM10_IRQn,0xFF);
	
	m_dataReadySem = xSemaphoreCreateBinary();
}

int SPI1Class::ReadWrite(unsigned char* outReadData,unsigned char* writeData,int byteRwLength)
{
	m_txBuf[0] = 0xAA;
	m_txBuf[1] = 0xAA;
	m_txBuf[2] = 0xAA;
	m_txBuf[3] = 0xAA;

//	DMA_Cmd(DMA2_Stream3,DISABLE);
//	DMA_SetCurrDataCounter(DMA2_Stream3,4);
//	DMA_ClearFlag(DMA2_Stream3,DMA_FLAG_TCIF3);
//	DMA_Cmd(DMA2_Stream3,ENABLE);
//	
//	GPIO_SetBits(GPIOC,GPIO_PinSource1);
//	vTaskDelay(1);
//	GPIO_ResetBits(GPIOC,GPIO_PinSource1);
	
	timerStart();
	
	return 0;
}
void SPI1Class::timerStart(){
	TIM_Cmd(TIM1,ENABLE);
}

void SPI1Class::waitNewData(){
	xSemaphoreTake(m_dataReadySem,0);
}
unsigned short* SPI1Class::getRxBuf(){
	return m_rxBuf; 
}
void SPI1Class::TIM1_UP_TIM10_IRQHandler(){
	if(TIM_GetITStatus(TIM1,TIM_IT_Update)!=RESET){
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		portBASE_TYPE pxHigherPriorityTaskWoken = pdTRUE;
		xSemaphoreGiveFromISR(m_dataReadySem,&pxHigherPriorityTaskWoken);
	}
}
