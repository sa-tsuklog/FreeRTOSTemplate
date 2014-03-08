
#include "stdlib.h"
#include "stdio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "projdefs.h"
#include "HalSpi2.h"

#define SPI_BUFFERSIZE 32

char* spi2TxBuf;
char* spi2RxBuf;

xSemaphoreHandle rwSem=NULL;
xSemaphoreHandle dataReadySem=NULL;

void initSpi2(){
	spi2TxBuf = (char*)malloc(sizeof(char)*SPI_BUFFERSIZE);
	spi2RxBuf = (char*)malloc(sizeof(char)*SPI_BUFFERSIZE);
	
	if(spi2RxBuf == NULL || spi2TxBuf == NULL){
		printf("malloc error at initSpi2\n\r");
		while(1){}
	}
	
	rwSem = xSemaphoreCreateBinary();
	dataReadySem = xSemaphoreCreateBinary();
	if(rwSem == NULL || dataReadySem == NULL){
		printf("malloc error at initSpi2\n\r");
		while(1){}
	}
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	GPIO_DeInit(GPIOB);
	
	GPIO_InitTypeDef pb12def;
	
	GPIO_StructInit(&pb12def);
	pb12def.GPIO_Pin = GPIO_Pin_12;
	pb12def.GPIO_Mode = GPIO_Mode_OUT;
	pb12def.GPIO_OType = GPIO_OType_PP;
	pb12def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pb12def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&pb12def);
	
	GPIO_InitTypeDef pe9def;
	//GPIO_DeInit(GPIOE);
	GPIO_StructInit(&pe9def);
	pe9def.GPIO_Pin = GPIO_Pin_9;
	pe9def.GPIO_Mode = GPIO_Mode_OUT;
	pe9def.GPIO_OType = GPIO_OType_PP;
	pe9def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pe9def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&pe9def);
	
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
	
	GPIO_InitTypeDef pa15def;
		
	GPIO_StructInit(&pa15def);
	pa15def.GPIO_Pin = GPIO_Pin_15;
	pa15def.GPIO_Mode = GPIO_Mode_AF;
	pa15def.GPIO_OType = GPIO_OType_PP;
	pa15def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pa15def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&pa15def);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_SPI2);
	GPIO_WriteBit(GPIOE,GPIO_Pin_9,Bit_SET);
	//GPIO_SetBits(GPIOB,GPIO_Pin_12);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);
	
	
	SPI_InitTypeDef spi2;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	spi2.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	spi2.SPI_CPHA = SPI_CPHA_2Edge;
	spi2.SPI_CPOL = SPI_CPOL_High;
	spi2.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi2.SPI_DataSize = SPI_DataSize_8b;
	spi2.SPI_FirstBit = SPI_FirstBit_MSB;
	spi2.SPI_Mode = SPI_Mode_Master;
	spi2.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPI2,&spi2);
	
	SPI_SSOutputCmd(SPI2,ENABLE);
	SPI_NSSInternalSoftwareConfig(SPI2,SPI_NSSInternalSoft_Reset);
	SPI_I2S_DMACmd(SPI2,SPI_DMAReq_Rx|SPI_DMAReq_Tx,ENABLE);
	SPI_Cmd(SPI2,ENABLE);
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	
	DMA_InitTypeDef dma1_3;
	DMA_StructInit(&dma1_3);
	dma1_3.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	dma1_3.DMA_Memory0BaseAddr = (uint32_t)spi2RxBuf;
	dma1_3.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma1_3.DMA_BufferSize = SPI_BUFFERSIZE;
	dma1_3.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_3.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_3.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma1_3.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma1_3.DMA_Mode = DMA_Mode_Normal;
	dma1_3.DMA_Priority = DMA_Priority_Medium;
	
	dma1_3.DMA_Channel = DMA_Channel_0;
	DMA_Init(DMA1_Stream3,&dma1_3);
	
	DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,ENABLE);
	
	
	DMA_InitTypeDef dma1_4;
	DMA_StructInit(&dma1_4);
	dma1_4.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	dma1_4.DMA_Memory0BaseAddr = (uint32_t)spi2TxBuf;
	dma1_4.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma1_4.DMA_BufferSize = SPI_BUFFERSIZE;
	dma1_4.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_4.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_4.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma1_4.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma1_4.DMA_Mode = DMA_Mode_Normal;
	dma1_4.DMA_Priority = DMA_Priority_Medium;
	
	dma1_4.DMA_Channel = DMA_Channel_0;
	DMA_Init(DMA1_Stream4,&dma1_4);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource14);
	
	
	EXTI_InitTypeDef exti14def;
	EXTI_StructInit(&exti14def);
	exti14def.EXTI_Line = EXTI_Line14;
	exti14def.EXTI_Mode = EXTI_Mode_Interrupt;
	exti14def.EXTI_Trigger = EXTI_Trigger_Falling;
	exti14def.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti14def);
	
	NVIC_InitTypeDef exti_nvicdef;
	exti_nvicdef.NVIC_IRQChannel  = EXTI15_10_IRQn;
	exti_nvicdef.NVIC_IRQChannelPreemptionPriority = 0x00;
	exti_nvicdef.NVIC_IRQChannelSubPriority = 0x00;
	exti_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&exti_nvicdef);
	NVIC_SetPriority(EXTI15_10_IRQn,0x08);
	
	NVIC_DisableIRQ(EXTI15_10_IRQn);
	
	NVIC_InitTypeDef dma_nvicdef;
	dma_nvicdef.NVIC_IRQChannel  = DMA1_Stream3_IRQn;
	dma_nvicdef.NVIC_IRQChannelPreemptionPriority = 0x00;
	dma_nvicdef.NVIC_IRQChannelSubPriority = 0x00;
	dma_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&dma_nvicdef);
	NVIC_SetPriority(DMA1_Stream3_IRQn,0x08);
}

void clearSemaphres(){
	xSemaphoreTake(dataReadySem,0);
	xSemaphoreTake(rwSem,0);
}

int spi2ReadWrite(unsigned char* outReadData,unsigned char* writeData,int byteRwLength){
	GPIO_WriteBit(GPIOE,GPIO_Pin_9,Bit_RESET);
	
	DMA_Cmd(DMA1_Stream3,DISABLE);
	DMA_Cmd(DMA1_Stream4,DISABLE);
	
	for(int i=0;i<byteRwLength && i < SPI_BUFFERSIZE;i++){
		spi2TxBuf[i] = writeData[i];
	}
	DMA_SetCurrDataCounter(DMA1_Stream3,byteRwLength);
	DMA_SetCurrDataCounter(DMA1_Stream4,byteRwLength);
	DMA_ClearITPendingBit(DMA1_Stream4,DMA_IT_TCIF4);
	
	
	DMA_Cmd(DMA1_Stream3,ENABLE);
	DMA_Cmd(DMA1_Stream4,ENABLE);
	
	xSemaphoreTake(rwSem,portMAX_DELAY);
	
	GPIO_WriteBit(GPIOE,GPIO_Pin_9,Bit_SET);
	
	for(int i=0;i<byteRwLength && i < SPI_BUFFERSIZE;i++){
		outReadData[i] = spi2RxBuf[i];
	}
	
	GPIO_WriteBit(GPIOE,GPIO_Pin_9,Bit_RESET);
	
	if(byteRwLength < SPI_BUFFERSIZE){
		return byteRwLength;
	}else{
		return SPI_BUFFERSIZE;
	}
}

void waitForDataReady(){
	EXTI_ClearITPendingBit(EXTI_Line14);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	if(dataReadySem!=NULL){
		xSemaphoreTake(dataReadySem,portMAX_DELAY);
	}
}

void myEXTI14_IRQHandler(){
	if(dataReadySem!=NULL){
		xSemaphoreGiveFromISR(dataReadySem,pdTRUE);
	}
	NVIC_DisableIRQ(EXTI15_10_IRQn);
	portEND_SWITCHING_ISR(pdTRUE);
}

void myDMA1_Stream3_IRQHandler(){
	if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3)!=RESET){
		DMA_ClearITPendingBit(DMA1_Stream3,DMA_IT_TCIF3);
		xSemaphoreGiveFromISR(rwSem,pdTRUE);
		portEND_SWITCHING_ISR(pdTRUE);
	}
}

