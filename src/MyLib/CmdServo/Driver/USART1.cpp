/*
 * USART1.cpp
 *
 *  Created on: 2014/04/25
 *      Author: sa
 */


#include "USART1.h"
#include "FreeRTOS.h"
#include "task.h"
#include "GeneralConfig.h"

USART1Class::USART1Class(){
	txQueue = xQueueCreate(TX_BUFFERSIZE,sizeof(char));
	vQueueAddToRegistry(txQueue,"u1tx");
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	GPIO_InitTypeDef pb6def;

	GPIO_StructInit(&pb6def);
	pb6def.GPIO_Pin = GPIO_Pin_6;
	pb6def.GPIO_Mode = GPIO_Mode_AF;
	pb6def.GPIO_OType = GPIO_OType_PP;
	pb6def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pb6def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&pb6def);

	GPIO_InitTypeDef pb7def;
	GPIO_StructInit(&pb7def);
	pb7def.GPIO_Pin = GPIO_Pin_7;
	pb7def.GPIO_Mode = GPIO_Mode_AF;
	pb7def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pb7def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&pb7def);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6,  GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

	USART_InitTypeDef usart1;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	USART_DeInit(USART1);

	USART_StructInit(&usart1);
	usart1.USART_BaudRate = 115200;
	usart1.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart1.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	usart1.USART_Parity = USART_Parity_No;
	usart1.USART_StopBits = USART_StopBits_1;
	usart1.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART1,&usart1);

	USART_DMACmd(USART1,USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);

	USART_Cmd(USART1, ENABLE);


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	
	

	DMA_InitTypeDef dma2_7;
	DMA_StructInit(&dma2_7);
	dma2_7.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
	dma2_7.DMA_Memory0BaseAddr = (uint32_t)m_txBuf;
	dma2_7.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma2_7.DMA_BufferSize = 1;
	dma2_7.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma2_7.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma2_7.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma2_7.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma2_7.DMA_Mode = DMA_Mode_Normal;
	dma2_7.DMA_Priority = DMA_Priority_High;

	dma2_7.DMA_Channel = DMA_Channel_4;
	DMA_Init(DMA2_Stream7,&dma2_7);

	DMA_InitTypeDef dma2_5;
	DMA_StructInit(&dma2_5);
	dma2_5.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
	dma2_5.DMA_Memory0BaseAddr = (uint32_t)m_rxBuf;
	dma2_5.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma2_5.DMA_BufferSize = RX_BUFFERSIZE;
	dma2_5.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma2_5.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma2_5.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma2_5.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma2_5.DMA_Mode = DMA_Mode_Circular;
	dma2_5.DMA_Priority = DMA_Priority_Low;

	dma2_5.DMA_Channel = DMA_Channel_4;
	DMA_Init(DMA2_Stream5,&dma2_5);

	DMA_Cmd(DMA2_Stream5,ENABLE);


	for(int i=0;i<RX_BUFFERSIZE;i++){
		m_rxBuf[i] = 0;
	}
}

void USART1Class::Tx()
{
	int numTx;
	portTickType xLastWakeTime = xTaskGetTickCount();
	vTaskDelay(MS_INITIAL_DELAY);
	
	while(1){
		if(DMA_GetCmdStatus(DMA2_Stream7)==DISABLE && (numTx = uxQueueMessagesWaiting(txQueue)) != 0){
			for(int i=0;i<numTx;i++){
				char c;
				xQueueReceive(txQueue,&c,0);
				m_txBuf[i] = c;
			}
	
			DMA_SetCurrDataCounter(DMA2_Stream7,numTx);
			USART_ClearFlag(USART1,USART_SR_TC);
			DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
			DMA_Cmd(DMA2_Stream7,ENABLE);
		}
		vTaskDelayUntil(&xLastWakeTime,1);
	}
}

inline void USART1Class::uputchar(char c){
	xQueueSendToBack(txQueue,&c,0);
}

void USART1Class::Rx()
{
	int rxBufIndex = 0;
	int lineBufIndex = 0;

	char c;
	while(1){
		//uint32_t start;
		//uint32_t end;
		//start = TIM2Class::GetInstance()->getUsTime();
		while( (c = m_rxBuf[rxBufIndex]) != 0 ){
			m_rxBuf[rxBufIndex]=0;
			if(c=='\r'){
			}else if(c=='\n'){
				m_lineBuf[lineBufIndex]=0;
	
				
				//TODO: add callback function here.
				//SerialCommand::GetInstance()->handleSerialCommand(m_lineBuf);
				
				lineBufIndex=0;
	
			}else{
				m_lineBuf[lineBufIndex]=c;
				lineBufIndex++;
			}
	
			rxBufIndex=(rxBufIndex+1)%RX_BUFFERSIZE;
		}
		//end = TIM2Class::GetInstance()->getUsTime();
		//printf("rx1 %d[us]",end - start);
		vTaskDelay(100);
	}
}

void USART1Class::prvTxTask(void *pvParameters)
{
	USART1Class::GetInstance()->Tx();
}

void USART1Class::prvRxTask(void *pvParameters)
{
	USART1Class::GetInstance()->Rx();
}


