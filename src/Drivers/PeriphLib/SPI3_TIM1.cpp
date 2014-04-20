#include "stdio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "SPI3_TIM1.h"

#include "task.h"
#include "queue.h"


SemaphoreHandle_t SPI3_TIM1_dataReadySem;

SPI3Class::SPI3Class(){
}

void SPI3Class::init(){
	SPI3_TIM1_dataReadySem= xSemaphoreCreateBinary();
	
	//////////////////////
	//GPIO Setting
	//////////////////////
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);

	GPIO_InitTypeDef pc10def;
	GPIO_StructInit(&pc10def);
	pc10def.GPIO_Pin = GPIO_Pin_10;
	pc10def.GPIO_Mode = GPIO_Mode_AF;
	pc10def.GPIO_OType = GPIO_OType_PP;
	pc10def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pc10def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&pc10def);

	GPIO_InitTypeDef pc11def;
	GPIO_StructInit(&pc11def);
	pc11def.GPIO_Pin = GPIO_Pin_11;
	pc11def.GPIO_Mode = GPIO_Mode_AF;
	pc11def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pc11def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&pc11def);

	GPIO_InitTypeDef pc12def;
	GPIO_StructInit(&pc12def);
	pc12def.GPIO_Pin = GPIO_Pin_12;
	pc12def.GPIO_Mode = GPIO_Mode_AF;
	pc12def.GPIO_OType = GPIO_OType_PP;
	pc12def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pc12def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&pc12def);
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SPI3);
	
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
	
	//trigger in
	GPIO_InitTypeDef pe13def;
	GPIO_StructInit(&pe13def);
	pe13def.GPIO_Pin = GPIO_Pin_13;
	pe13def.GPIO_Mode = GPIO_Mode_IN;
	pe13def.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOE,&pe13def);

	
	///////////////////////////
	//SPI Setting
	///////////////////////////
	SPI_InitTypeDef spi3def;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);
	spi3def.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	spi3def.SPI_CPHA = SPI_CPHA_2Edge;
	spi3def.SPI_CPOL = SPI_CPOL_High;
	spi3def.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi3def.SPI_DataSize = SPI_DataSize_16b;
	spi3def.SPI_FirstBit = SPI_FirstBit_MSB;
	spi3def.SPI_Mode = SPI_Mode_Master;
	spi3def.SPI_NSS = SPI_NSS_Hard;
	
	SPI_Init(SPI3,&spi3def);
	SPI_SSOutputCmd(SPI3,ENABLE);
	//SPI_I2S_DMACmd(SPI3,SPI_DMAReq_Rx|SPI_DMAReq_Tx,ENABLE);
	SPI_I2S_DMACmd(SPI3,SPI_DMAReq_Rx,ENABLE);
	SPI_Cmd(SPI3,ENABLE);
	
	///////////////////////////
	//TIM1 Setting
	///////////////////////////
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
	
	//TIM DMA
	TIM_OCInitTypeDef oc1;
	TIM_OCStructInit(&oc1);
	oc1.TIM_Pulse = 350;
	TIM_OC1Init(TIM1,&oc1);
	
	//update int
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	TIM_DMACmd(TIM1,TIM_DMA_CC1,ENABLE);
	
	/////////////////////////////
	//DMA Setting
	/////////////////////////////
	//SPI3RX
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	DMA_InitTypeDef dma1_0;
	DMA_StructInit(&dma1_0);
	dma1_0.DMA_PeripheralBaseAddr = (uint32_t)&(SPI3->DR);
	dma1_0.DMA_Memory0BaseAddr = (uint32_t)m_rxBuf;
	dma1_0.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma1_0.DMA_BufferSize = 2;
	dma1_0.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_0.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_0.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma1_0.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma1_0.DMA_Mode = DMA_Mode_Circular;
	dma1_0.DMA_Priority = DMA_Priority_Medium;

	dma1_0.DMA_Channel = DMA_Channel_0;
	DMA_Init(DMA1_Stream0,&dma1_0);
	DMA_Cmd(DMA1_Stream0,ENABLE);
	
	//TIM1_CH1
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	DMA_InitTypeDef dma2_6;
	DMA_StructInit(&dma2_6);
	dma2_6.DMA_PeripheralBaseAddr = (uint32_t)&(SPI3->DR);
	dma2_6.DMA_Memory0BaseAddr = (uint32_t)m_txBuf;
	dma2_6.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma2_6.DMA_BufferSize = 2;
	dma2_6.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma2_6.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma2_6.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma2_6.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma2_6.DMA_Mode = DMA_Mode_Circular;
	dma2_6.DMA_Priority = DMA_Priority_Medium;

	dma2_6.DMA_Channel = DMA_Channel_0;
	DMA_Init(DMA2_Stream6,&dma2_6);
	DMA_Cmd(DMA2_Stream6,ENABLE);
	
	
	////////////////////////////
	//Interrupt
	/////////////////////////////
	//DMA
	NVIC_InitTypeDef dma_nvicdef;
	dma_nvicdef.NVIC_IRQChannel  = DMA2_Stream6_IRQn;
	dma_nvicdef.NVIC_IRQChannelPreemptionPriority = 0x00;
	dma_nvicdef.NVIC_IRQChannelSubPriority = 0x00;
	dma_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&dma_nvicdef);
	NVIC_SetPriority(DMA2_Stream6_IRQn,0x08);
	
	//TIM
	NVIC_InitTypeDef timitdef;
	timitdef.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	timitdef.NVIC_IRQChannelPreemptionPriority = 0x00;
	timitdef.NVIC_IRQChannelSubPriority = 0x00;
	timitdef.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&timitdef);
	
	NVIC_SetPriority(TIM1_UP_TIM10_IRQn,0xFF);
	//NVIC_SetPriority(TIM1_CC_IRQn,0xFF);

	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);	
	//NVIC_EnableIRQ(TIM1_CC_IRQn);

	//Trigger
	EXTI_InitTypeDef exti13def;
	EXTI_StructInit(&exti13def);
	exti13def.EXTI_Line = EXTI_Line13;
	exti13def.EXTI_Mode = EXTI_Mode_Interrupt;
	exti13def.EXTI_Trigger = EXTI_Trigger_Rising;
	exti13def.EXTI_LineCmd =ENABLE;
	EXTI_Init(&exti13def);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource13);
	
	
	NVIC_InitTypeDef exti13itdef;
	exti13itdef.NVIC_IRQChannel = EXTI15_10_IRQn;
	exti13itdef.NVIC_IRQChannelPreemptionPriority = 0x00;
	exti13itdef.NVIC_IRQChannelSubPriority = 0x00;
	exti13itdef.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&exti13itdef);
	NVIC_SetPriority(EXTI15_10_IRQn,0xFF);
	
	///////////////
	//EXTI 0
	////////////////
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef pa0def;
	GPIO_StructInit(&pa0def);
	pa0def.GPIO_Pin = GPIO_Pin_0;
	pa0def.GPIO_Mode = GPIO_Mode_IN;
	pa0def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pa0def.GPIO_Speed = GPIO_Speed_100MHz;
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
	
	NVIC_SetPriority(EXTI0_IRQn,0xFF);
}


void SPI3Class::setSingleTransactionMode(){
	TIM_Cmd(TIM1,DISABLE);
	
	TIM_TimeBaseInitTypeDef tim1;
	TIM_TimeBaseStructInit(&tim1);
	tim1.TIM_ClockDivision = TIM_CKD_DIV2;
	tim1.TIM_Prescaler = 0;
	tim1.TIM_Period = 1450-1;
	tim1.TIM_CounterMode = TIM_CounterMode_Up;
	tim1.TIM_RepetitionCounter = 1;
	TIM_TimeBaseInit(TIM1,&tim1);
	
	DMA_Cmd(DMA1_Stream0,DISABLE);
	DMA_InitTypeDef dma1_0;
	DMA_StructInit(&dma1_0);
	dma1_0.DMA_PeripheralBaseAddr = (uint32_t)&(SPI3->DR);
	dma1_0.DMA_Memory0BaseAddr = (uint32_t)m_rxBuf;
	dma1_0.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma1_0.DMA_BufferSize = 2;
	dma1_0.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_0.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_0.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma1_0.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma1_0.DMA_Mode = DMA_Mode_Circular;
	dma1_0.DMA_Priority = DMA_Priority_Medium;

	dma1_0.DMA_Channel = DMA_Channel_0;
	DMA_Init(DMA1_Stream0,&dma1_0);
	DMA_Cmd(DMA1_Stream0,ENABLE);
	
	DMA_Cmd(DMA2_Stream6,DISABLE);
	
	DMA_InitTypeDef dma2_6;
	DMA_StructInit(&dma2_6);
	dma2_6.DMA_PeripheralBaseAddr = (uint32_t)&(SPI3->DR);
	dma2_6.DMA_Memory0BaseAddr = (uint32_t)m_txBuf;
	dma2_6.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma2_6.DMA_BufferSize = 2;
	dma2_6.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma2_6.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma2_6.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma2_6.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma2_6.DMA_Mode = DMA_Mode_Circular;
	dma2_6.DMA_Priority = DMA_Priority_Medium;

	dma2_6.DMA_Channel = DMA_Channel_0;
	DMA_Init(DMA2_Stream6,&dma2_6);
	DMA_Cmd(DMA2_Stream6,ENABLE);
}

void SPI3Class::setContinuousMode(){
	TIM_Cmd(TIM1,DISABLE);
		
	TIM_TimeBaseInitTypeDef tim1;
	TIM_TimeBaseStructInit(&tim1);
	tim1.TIM_ClockDivision = TIM_CKD_DIV2;
	tim1.TIM_Prescaler = 0;
	tim1.TIM_Period = 1450-1;
	tim1.TIM_CounterMode = TIM_CounterMode_Up;
	tim1.TIM_RepetitionCounter = 18;
	TIM_TimeBaseInit(TIM1,&tim1);
	
	DMA_Cmd(DMA1_Stream0,DISABLE);
	DMA_InitTypeDef dma1_0;
	DMA_StructInit(&dma1_0);
	dma1_0.DMA_PeripheralBaseAddr = (uint32_t)&(SPI3->DR);
	dma1_0.DMA_Memory0BaseAddr = (uint32_t)m_rxBuf;
	dma1_0.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma1_0.DMA_BufferSize = 19;
	dma1_0.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_0.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_0.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma1_0.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma1_0.DMA_Mode = DMA_Mode_Circular;
	dma1_0.DMA_Priority = DMA_Priority_Medium;

	dma1_0.DMA_Channel = DMA_Channel_0;
	DMA_Init(DMA1_Stream0,&dma1_0);
	DMA_Cmd(DMA1_Stream0,ENABLE);
	
	DMA_Cmd(DMA2_Stream6,DISABLE);
	DMA_InitTypeDef dma2_6;
	DMA_StructInit(&dma2_6);
	dma2_6.DMA_PeripheralBaseAddr = (uint32_t)&(SPI3->DR);
	dma2_6.DMA_Memory0BaseAddr = (uint32_t)m_txBuf;
	dma2_6.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma2_6.DMA_BufferSize = 19;
	dma2_6.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma2_6.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma2_6.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma2_6.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma2_6.DMA_Mode = DMA_Mode_Circular;
	dma2_6.DMA_Priority = DMA_Priority_Medium;

	dma2_6.DMA_Channel = DMA_Channel_0;
	DMA_Init(DMA2_Stream6,&dma2_6);
	DMA_Cmd(DMA2_Stream6,ENABLE);
}


void SPI3Class::timerStart(){
	xSemaphoreTake(SPI3_TIM1_dataReadySem,0);
	xSemaphoreTake(SPI3_TIM1_dataReadySem,0);
	TIM_Cmd(TIM1,ENABLE);
}

void SPI3Class::waitNewData(){
	GPIO_SetBits(GPIOC,GPIO_Pin_1);
	//xSemaphoreTake(SPI3_TIM1_dataReadySem,portMAX_DELAY);
	xSemaphoreTake(SPI3_TIM1_dataReadySem,1000);
	GPIO_ResetBits(GPIOC,GPIO_Pin_1);
	
}
void SPI3Class::setTxBuf(int index,unsigned short data){
	if(index < SPI_BUFFERSIZE){
		m_txBuf[index] = data;
	}
}
unsigned short* SPI3Class::getRxBuf(){
	return m_rxBuf; 
}

void SPI3Class::write16(unsigned char address,unsigned short data){
	m_txBuf[0] = 0x8000 | address<<8 | (0xFF & (data));
	m_txBuf[1] = 0x8100 | address<<8 | (0xFF & (data>>8));
	setSingleTransactionMode();
	timerStart();
	waitNewData();
}
unsigned short SPI3Class::read16(unsigned char address){
	m_txBuf[0] = address<<8;
	m_txBuf[1] = 0x0;
	setSingleTransactionMode();
	timerStart();
	waitNewData();
	
	return SPI3->DR;
}

void SPI3Class::TIM1_UP_TIM10_IRQHandler(){
	portBASE_TYPE pxHigherPriorityTaskWoken = pdTRUE;
	xSemaphoreGiveFromISR(SPI3_TIM1_dataReadySem,&pxHigherPriorityTaskWoken);
}