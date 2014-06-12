#include "stdio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "SPI2_TIM8.h"

#include "task.h"
#include "queue.h"


SemaphoreHandle_t SPI2_TIM8_dataReadySem;

SPI2Class::SPI2Class(){
}

void SPI2Class::init(){
	SPI2_TIM8_dataReadySem= xSemaphoreCreateBinary();
	xSemaphoreTake(SPI2_TIM8_dataReadySem,0);
	xSemaphoreTake(SPI2_TIM8_dataReadySem,0);
	
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
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	//trigger in
	GPIO_InitTypeDef pc6def;
	GPIO_StructInit(&pc6def);
	pc6def.GPIO_Pin = GPIO_Pin_6;
	pc6def.GPIO_Mode = GPIO_Mode_IN;
	pc6def.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOC,&pc6def);
	
	//NSS port setting	
	GPIO_InitTypeDef pc7def;
	GPIO_StructInit(&pc7def);
	pc7def.GPIO_Pin = GPIO_Pin_7;
	pc7def.GPIO_Mode = GPIO_Mode_AF;
	pc7def.GPIO_OType = GPIO_OType_PP;
	pc7def.GPIO_Speed = GPIO_Speed_25MHz;
	pc7def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&pc7def);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8);
	
	//RST port setting
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	GPIO_InitTypeDef pd10def;
	GPIO_StructInit(&pd10def);
	pd10def.GPIO_Pin = GPIO_Pin_10;
	pd10def.GPIO_Mode = GPIO_Mode_OUT;
	pd10def.GPIO_OType = GPIO_OType_PP;
	pd10def.GPIO_Speed = GPIO_Speed_25MHz;
	pd10def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&pd10def);
	GPIO_ResetBits(GPIOD,GPIO_Pin_10);
	
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
	
	///////////////////////////
	//TIM8 Setting
	///////////////////////////
	//timebase
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
	TIM_TimeBaseInitTypeDef tim8def;
	TIM_TimeBaseStructInit(&tim8def);
	tim8def.TIM_ClockDivision = TIM_CKD_DIV2;
	tim8def.TIM_Prescaler = 0;
	tim8def.TIM_Period = 1450-1;
	tim8def.TIM_CounterMode = TIM_CounterMode_Up;
	tim8def.TIM_RepetitionCounter = 3;
	TIM_TimeBaseInit(TIM8,&tim8def);
	TIM_SelectOnePulseMode(TIM8,TIM_OPMode_Single);
	
	//NSS generation
	TIM_OCInitTypeDef oc2;
	TIM_OCStructInit(&oc2);
	oc2.TIM_OCMode = TIM_OCMode_PWM1;
	oc2.TIM_OCPolarity = TIM_OCPolarity_High;
	oc2.TIM_OutputState = TIM_OutputState_Enable;
	oc2.TIM_Pulse = 350-1;
	TIM_OC2Init(TIM8,&oc2);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
	
	//TIM DMA
	TIM_OCInitTypeDef oc3;
	TIM_OCStructInit(&oc3);
	oc3.TIM_Pulse = 350;
	TIM_OC3Init(TIM8,&oc3);
	
	//update int
	TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE);
	TIM_DMACmd(TIM8,TIM_DMA_CC3,ENABLE);
	
	/////////////////////////////
	//DMA Setting
	/////////////////////////////
	//SPI2RX
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	DMA_InitTypeDef dma1_3;
	DMA_StructInit(&dma1_3);
	dma1_3.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	dma1_3.DMA_Memory0BaseAddr = (uint32_t)m_rxBuf;
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
	dma2_2.DMA_Memory0BaseAddr = (uint32_t)m_txBuf;
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
	NVIC_SetPriority(DMA2_Stream6_IRQn,0x08);
	
	//TIM
	NVIC_InitTypeDef timitdef;
	timitdef.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	timitdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	timitdef.NVIC_IRQChannelSubPriority = 0xFF;
	timitdef.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&timitdef);
	
	NVIC_SetPriority(TIM8_UP_TIM13_IRQn,0xFF);
	//NVIC_SetPriority(TIM8_CC_IRQn,0xFF);

	NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);	
	//NVIC_EnableIRQ(TIM8_CC_IRQn);

	//Trigger
	EXTI_InitTypeDef exti6def;
	EXTI_StructInit(&exti6def);
	exti6def.EXTI_Line = EXTI_Line6;
	exti6def.EXTI_Mode = EXTI_Mode_Interrupt;
	exti6def.EXTI_Trigger = EXTI_Trigger_Rising;
	exti6def.EXTI_LineCmd =ENABLE;
	EXTI_Init(&exti6def);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource6);
	
	
	NVIC_InitTypeDef exti6itdef;
	exti6itdef.NVIC_IRQChannel = EXTI9_5_IRQn;
	exti6itdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	exti6itdef.NVIC_IRQChannelSubPriority = 0xFF;
	exti6itdef.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&exti6itdef);
	NVIC_SetPriority(EXTI9_5_IRQn,0xFF);
	
	vTaskDelay(100);
	GPIO_SetBits(GPIOD,GPIO_Pin_10);
	vTaskDelay(500);
}

void SPI2Class::setSingleTransactionMode(){
	TIM_Cmd(TIM8,DISABLE);
	
	TIM_TimeBaseInitTypeDef tim8def;
	TIM_TimeBaseStructInit(&tim8def);
	tim8def.TIM_ClockDivision = TIM_CKD_DIV2;
	tim8def.TIM_Prescaler = 0;
	tim8def.TIM_Period = 1450-1;
	tim8def.TIM_CounterMode = TIM_CounterMode_Up;
	tim8def.TIM_RepetitionCounter = 1;
	TIM_TimeBaseInit(TIM8,&tim8def);
	
	DMA_Cmd(DMA1_Stream3,DISABLE);
	DMA_InitTypeDef dma1_3;
	DMA_StructInit(&dma1_3);
	dma1_3.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	dma1_3.DMA_Memory0BaseAddr = (uint32_t)m_rxBuf;
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
	
	DMA_Cmd(DMA2_Stream2,DISABLE);
	
	DMA_InitTypeDef dma2_2;
	DMA_StructInit(&dma2_2);
	dma2_2.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	dma2_2.DMA_Memory0BaseAddr = (uint32_t)m_txBuf;
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
}

void SPI2Class::setContinuousMode(){
	TIM_Cmd(TIM8,DISABLE);
		
	TIM_TimeBaseInitTypeDef tim8def;
	TIM_TimeBaseStructInit(&tim8def);
	tim8def.TIM_ClockDivision = TIM_CKD_DIV2;
	tim8def.TIM_Prescaler = 0;
	tim8def.TIM_Period = 1450-1;
	tim8def.TIM_CounterMode = TIM_CounterMode_Up;
	tim8def.TIM_RepetitionCounter = 18;
	TIM_TimeBaseInit(TIM8,&tim8def);
	
	DMA_Cmd(DMA1_Stream3,DISABLE);
	DMA_InitTypeDef dma1_3;
	DMA_StructInit(&dma1_3);
	dma1_3.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	dma1_3.DMA_Memory0BaseAddr = (uint32_t)m_rxBuf;
	dma1_3.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma1_3.DMA_BufferSize = 19;
	dma1_3.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_3.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_3.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma1_3.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma1_3.DMA_Mode = DMA_Mode_Circular;
	dma1_3.DMA_Priority = DMA_Priority_Medium;

	dma1_3.DMA_Channel = DMA_Channel_0;
	DMA_Init(DMA1_Stream3,&dma1_3);
	DMA_Cmd(DMA1_Stream3,ENABLE);
	
	DMA_Cmd(DMA2_Stream2,DISABLE);
	DMA_InitTypeDef dma2_2;
	DMA_StructInit(&dma2_2);
	dma2_2.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	dma2_2.DMA_Memory0BaseAddr = (uint32_t)m_txBuf;
	dma2_2.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma2_2.DMA_BufferSize = 19;
	dma2_2.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma2_2.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma2_2.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma2_2.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma2_2.DMA_Mode = DMA_Mode_Circular;
	dma2_2.DMA_Priority = DMA_Priority_Medium;

	dma2_2.DMA_Channel = DMA_Channel_0;
	DMA_Init(DMA2_Stream2,&dma2_2);
	DMA_Cmd(DMA2_Stream2,ENABLE);
	
	NVIC_EnableIRQ(EXTI9_5_IRQn);
}


void SPI2Class::timerStart(){
	//xSemaphoreTake(SPI2_TIM8_dataReadySem,0);
	portBASE_TYPE xSwitchRequired;
	xSemaphoreTakeFromISR(SPI2_TIM8_dataReadySem,&xSwitchRequired);
	TIM_Cmd(TIM8,ENABLE);
}

void SPI2Class::waitNewData(){
	//xSemaphoreTake(SPI2_TIM8_dataReadySem,portMAX_DELAY);
	xSemaphoreTake(SPI2_TIM8_dataReadySem,0);
	xSemaphoreTake(SPI2_TIM8_dataReadySem,portMAX_DELAY);
}
void SPI2Class::setTxBuf(int index,unsigned short data){
	if(index < SPI_BUFFERSIZE){
		m_txBuf[index] = data;
	}
}
unsigned short* SPI2Class::getRxBuf(){
	return m_rxBuf; 
}

void SPI2Class::write16(unsigned char address,unsigned short data){
	m_txBuf[0] = 0x8000 | address<<8 | (0xFF & (data));
	m_txBuf[1] = 0x8100 | address<<8 | (0xFF & (data>>8));
	setSingleTransactionMode();
	
	timerStart();
	waitNewData();
}
unsigned short SPI2Class::read16(unsigned char address){
	m_txBuf[0] = address<<8;
	m_txBuf[1] = 0x0;
	setSingleTransactionMode();
	
	timerStart();
	waitNewData();
	
	return m_rxBuf[1];
}

void SPI2Class::myEXTI6_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line6);
	SPI2Class::GetInstance()->timerStart();
}

void SPI2Class::myTIM8_CC3_IRQHandler(){
	if(TIM_GetITStatus(TIM8,TIM_IT_CC3)!=RESET){
		TIM_ClearITPendingBit(TIM8,TIM_IT_CC3);
	}
}

void SPI2Class::myTIM8_IRQHandler(){
	portBASE_TYPE xSwitchRequired;
	
	TIM_ClearITPendingBit(TIM8,TIM_IT_Update);
	xSemaphoreGiveFromISR(SPI2_TIM8_dataReadySem,&xSwitchRequired);
	portEND_SWITCHING_ISR(xSwitchRequired );
}
