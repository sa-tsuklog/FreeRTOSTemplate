/*
 * USART3.cpp
 *
 *  Created on: 2014/04/27
 *      Author: sa
 */


#include <pch.h>
#include "USART3.h"
#include "MyLib/Stdout/SerialCommand.h"

USART3Class::USART3Class(){
	m_queue3 = xQueueCreate(TX_BUFFERSIZE,sizeof(char));
	vQueueAddToRegistry(m_queue3,"u3tx");
	txCompleteSem = xSemaphoreCreateBinary();
	
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

	USART_DMACmd(USART3,USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);

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

	DMA_InitTypeDef dma1_1;
	DMA_StructInit(&dma1_1);
	dma1_1.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
	dma1_1.DMA_Memory0BaseAddr = (uint32_t)m_rxBuf;
	dma1_1.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma1_1.DMA_BufferSize = RX_BUFFERSIZE;
	dma1_1.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_1.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_1.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma1_1.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma1_1.DMA_Mode = DMA_Mode_Circular;
	dma1_1.DMA_Priority = DMA_Priority_Low;

	dma1_1.DMA_Channel = DMA_Channel_4;
	DMA_Init(DMA1_Stream1,&dma1_1);
	
	DMA_Cmd(DMA1_Stream1,ENABLE);

	DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);
	
	NVIC_InitTypeDef dma1_4_nvicdef;
	dma1_4_nvicdef.NVIC_IRQChannel = DMA1_Stream4_IRQn;
	dma1_4_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	dma1_4_nvicdef.NVIC_IRQChannelSubPriority = 0xFF;
	dma1_4_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	NVIC_Init(&dma1_4_nvicdef);
	NVIC_SetPriority(DMA1_Stream4_IRQn,0xFF);

	for(int i=0;i<RX_BUFFERSIZE;i++){
		m_rxBuf[i] = 0;
	}
	
	echo = 0;
	rxBufIndex=0;
}

void USART3Class::Tx()
{
	portTickType xLastWakeTime = xTaskGetTickCount();
	xSemaphoreTake(txCompleteSem,0);
	while(1){
		int numTx=0;
		if(DMA_GetCmdStatus(DMA1_Stream4)==DISABLE && (numTx = uxQueueMessagesWaiting(m_queue3)) != 0){
			for(int i=0;i<numTx;i++){
				char c;
				xQueueReceive(m_queue3,&c,0);
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

char* USART3Class::readLine()
{
	int lineBufIndex = 0;

	char c;
	
	while(1){
		while( (c = m_rxBuf[rxBufIndex]) != 0 ){
			m_rxBuf[rxBufIndex]=0;
			rxBufIndex=(rxBufIndex+1)%RX_BUFFERSIZE;
			
			if(echo){
				if(c=='\r'){
					//do nothing
				}else if(c=='\n'){
					
				}else if(c=='\b'){
					putchar('\b');
					putchar(' ');
					putchar('\b');
					fflush(stdout);
				}else{
					putchar(c);
					fflush(stdout);
				}
			}
			
			
			if(c=='\r'){
			}else if(c=='\n'){
				if(lineBufIndex==0 || echo){
					putchar('\r');
					putchar('\n');
					fflush(stdout);
				}
				
				m_lineBuf[lineBufIndex]=0;
	
				//SerialCommand::GetInstance()->handleSerialCommand(m_lineBuf);
				lineBufIndex=0;
				return m_lineBuf;
			}else if(c=='\b'){
				if(lineBufIndex > 0){
					lineBufIndex--;
				}
			}else{
				
				m_lineBuf[lineBufIndex]=c;
				if(lineBufIndex<LINE_BUF_SIZE-1){
					lineBufIndex++;
				}
			}
		}
		
//		end = TIM2Class::GetInstance()->getUsTime();
//		printf("u3rx %d[us]\r\n",end-start);
		vTaskDelay(10);
	}
}

char USART3Class::getChar(){
	char c;
	while(1){
		if((c = m_rxBuf[rxBufIndex]) != 0){
			m_rxBuf[rxBufIndex]=0;
			rxBufIndex=(rxBufIndex+1)%RX_BUFFERSIZE;
			return c;
		}
		vTaskDelay(25);
	}
	return 0;
}

void USART3Class::prvTxTask(void *pvParameters)
{
	USART3Class::GetInstance()->Tx();
}

void USART3Class::myUSART3_IRQHandler(){
}

void USART3Class::myDMA1_Stream4IRQHandler(){
	if(DMA_GetITStatus(DMA1_Stream4,DMA_IT_TCIF4) != RESET){
		DMA_ClearITPendingBit(DMA1_Stream4,DMA_IT_TCIF4);
	
		portBASE_TYPE switchRequired;
		xSemaphoreGiveFromISR(txCompleteSem,&switchRequired);
		portEND_SWITCHING_ISR(switchRequired);
	}
}
