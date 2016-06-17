/*
 * USART3.cpp
 *
 *  Created on: 2014/04/27
 *      Author: sa
 */


#include <pch.h>
#include "USART3.h"

#include <pch.h>
#include "USART3.h"
#include "Common/Console/Ascii/Ascii.h"

USART3Class::USART3Class(){
	txQueue = xQueueCreate(StdoutConfig::TX_BUFFERSIZE,sizeof(char));
	if(txQueue == NULL){
		while(1){}
	}
	vQueueAddToRegistry(txQueue,"u3tx");
	
	txCompleteSem = xSemaphoreCreateBinary();
	
	rxQueue = xQueueCreate(StdoutConfig::RX_BUFFERSIZE,sizeof(char));
	if(rxQueue == NULL){
		while(1){}
	}
	vQueueAddToRegistry(rxQueue,"u3rx");
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

	GPIO_InitTypeDef pd8def;

	GPIO_StructInit(&pd8def);
	pd8def.GPIO_Pin = GPIO_Pin_8;
	pd8def.GPIO_Mode = GPIO_Mode_AF;
	pd8def.GPIO_OType = GPIO_OType_PP;
	pd8def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pd8def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD,&pd8def);

	GPIO_InitTypeDef pd9def;
	GPIO_StructInit(&pd9def);
	pd9def.GPIO_Pin = GPIO_Pin_9;
	pd9def.GPIO_Mode = GPIO_Mode_AF;
	pd9def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pd9def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD,&pd9def);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);


	USART_InitTypeDef usart3;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

	USART_StructInit(&usart3);
	usart3.USART_BaudRate = 115200;
	usart3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart3.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	usart3.USART_Parity = USART_Parity_No;
	usart3.USART_StopBits = USART_StopBits_1;
	usart3.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART3,&usart3);

	USART_DMACmd(USART3,USART_DMAReq_Tx, ENABLE);

	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	
	USART_Cmd(USART3, ENABLE);


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);


	DMA_InitTypeDef dma1_4;
	DMA_StructInit(&dma1_4);
	dma1_4.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
	dma1_4.DMA_Memory0BaseAddr = (uint32_t)m_txBuf;
	dma1_4.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma1_4.DMA_BufferSize = 1;
	dma1_4.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_4.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_4.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma1_4.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma1_4.DMA_Mode = DMA_Mode_Normal;
	dma1_4.DMA_Priority = DMA_Priority_High;

	dma1_4.DMA_Channel = DMA_Channel_7;
	DMA_Init(DMA1_Stream4,&dma1_4);

	DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);
	
	NVIC_InitTypeDef dma1_4_nvicdef;
	dma1_4_nvicdef.NVIC_IRQChannel = DMA1_Stream4_IRQn;
	dma1_4_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	dma1_4_nvicdef.NVIC_IRQChannelSubPriority = 0xFF;
	dma1_4_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	NVIC_Init(&dma1_4_nvicdef);
	NVIC_SetPriority(DMA1_Stream4_IRQn,0xFF);

	NVIC_InitTypeDef usart3Nvic;
	usart3Nvic.NVIC_IRQChannel = USART3_IRQn;
	usart3Nvic.NVIC_IRQChannelCmd = ENABLE;
	usart3Nvic.NVIC_IRQChannelPreemptionPriority = 0xFF;
	usart3Nvic.NVIC_IRQChannelSubPriority = 0xFF;
	NVIC_Init(&usart3Nvic);
	NVIC_SetPriority(USART3_IRQn,0xFF);
}

void USART3Class::Tx()
{
	portTickType xLastWakeTime = xTaskGetTickCount();
	xSemaphoreTake(txCompleteSem,0);
	while(1){
		int numTx=0;
		if(DMA_GetCmdStatus(DMA1_Stream4)==DISABLE && (numTx = uxQueueMessagesWaiting(txQueue)) != 0){
			for(int i=0;i<numTx;i++){
				char c;
				xQueueReceive(txQueue,&c,0);
				m_txBuf[i] = c;
			}

			DMA_SetCurrDataCounter(DMA1_Stream4,numTx);
			USART_ClearFlag(USART3,USART_SR_TC);
			DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);
			DMA_Cmd(DMA1_Stream4,ENABLE);
			
			xSemaphoreTake(txCompleteSem,portMAX_DELAY);
		}
		
		vTaskDelayUntil(&xLastWakeTime,1);
	}
}

uint32_t USART3Class::getChar(){
	uint8_t c;
	xQueueReceive(rxQueue,&c,portMAX_DELAY);
	return c;
}

void USART3Class::putChar(uint32_t c){
	xQueueSend(txQueue,&c,portMAX_DELAY);
}

void USART3Class::prvTxTask(void *pvParameters)
{
	USART3Class::GetInstance()->Tx();
}

void USART3Class::myDMA1_Stream4IRQHandler(){
	if(DMA_GetITStatus(DMA1_Stream4,DMA_IT_TCIF4) != RESET){
		DMA_ClearITPendingBit(DMA1_Stream4,DMA_IT_TCIF4);
	
		portBASE_TYPE switchRequired;
		xSemaphoreGiveFromISR(txCompleteSem,&switchRequired);
		portEND_SWITCHING_ISR(switchRequired);
	}
}

void USART3Class::myUSART3_IRQHandler(){
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET){
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		USART_ClearFlag(USART3,USART_FLAG_RXNE);
		
		uint8_t c = USART_ReceiveData(USART3);
		
		portBASE_TYPE dummy;
		xQueueSendFromISR(rxQueue,&c,&dummy);
	}
}
