/*
 * Stdout.cpp
 *
 *  Created on: 2014/02/10
 *      Author: sa
 */

#include "stdlib.h"
#include "stdio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "HalUsart.h"
#include "SerialCommand.hpp"

#define TX_BUFFERSIZE 1024
#define RX_BUFFERSIZE 64
#define LINE_BUF_SIZE 64

xQueueHandle usart2Que;
char* usart2TxBuf;
char* usart2RxBuf;

void initUart(USART_TypeDef* ch){
	usart2TxBuf = (char*)malloc(sizeof(char)*TX_BUFFERSIZE);
	usart2RxBuf = (char*)malloc(sizeof(char)*RX_BUFFERSIZE);
	usart2Que = xQueueCreate(TX_BUFFERSIZE,sizeof(char));

	
	if(usart2TxBuf ==NULL || usart2RxBuf ==NULL){
		while(1){}
	}

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef pa2def;

	GPIO_StructInit(&pa2def);
	pa2def.GPIO_Pin = GPIO_Pin_2;
	pa2def.GPIO_Mode = GPIO_Mode_AF;
	pa2def.GPIO_OType = GPIO_OType_PP;
	pa2def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pa2def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&pa2def);

	GPIO_InitTypeDef pa3def;
	GPIO_StructInit(&pa3def);
	pa3def.GPIO_Pin = GPIO_Pin_3;
	pa3def.GPIO_Mode = GPIO_Mode_AF;
	pa3def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pa3def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&pa3def);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	
	USART_InitTypeDef usart2;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	USART_StructInit(&usart2);
	usart2.USART_BaudRate = 115200;
	usart2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart2.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	usart2.USART_Parity = USART_Parity_No;
	usart2.USART_StopBits = USART_StopBits_1;
	usart2.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART2,&usart2);

	USART_DMACmd(USART2,USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);

	USART_Cmd(USART2, ENABLE);
	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

	
	DMA_InitTypeDef dma1_6;	
	DMA_StructInit(&dma1_6);
	dma1_6.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
	dma1_6.DMA_Memory0BaseAddr = (uint32_t)usart2TxBuf;
	dma1_6.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma1_6.DMA_BufferSize = 1;
	dma1_6.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_6.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_6.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma1_6.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma1_6.DMA_Mode = DMA_Mode_Normal;
	dma1_6.DMA_Priority = DMA_Priority_High;

	dma1_6.DMA_Channel = DMA_Channel_4;
	DMA_Init(DMA1_Stream6,&dma1_6);
	
	DMA_InitTypeDef dma1_5;
	DMA_StructInit(&dma1_5);
	dma1_5.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
	dma1_5.DMA_Memory0BaseAddr = (uint32_t)usart2RxBuf;
	dma1_5.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma1_5.DMA_BufferSize = RX_BUFFERSIZE;
	dma1_5.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_5.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_5.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma1_5.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma1_5.DMA_Mode = DMA_Mode_Circular;
	dma1_5.DMA_Priority = DMA_Priority_Low;

	dma1_5.DMA_Channel = DMA_Channel_4;
	DMA_Init(DMA1_Stream5,&dma1_5);
	
	DMA_Cmd(DMA1_Stream5,ENABLE);
}

void prvTxTask(void *pvParameters){
	portTickType xLastWakeTime = xTaskGetTickCount();

	if((USART_TypeDef*)pvParameters == USART2){
		while(1){
			int numTx;

			if(DMA_GetCmdStatus(DMA1_Stream6)==DISABLE && (numTx = uxQueueMessagesWaiting(usart2Que)) != 0){
				for(int i=0;i<numTx;i++){
					char c;
					portBASE_TYPE xStatus = xQueueReceive(usart2Que,&c,0);
					usart2TxBuf[i] = c;
				}

				DMA_SetCurrDataCounter(DMA1_Stream6,numTx);
				USART_ClearFlag(USART2,USART_SR_TC);
				DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_TCIF6);
				DMA_Cmd(DMA1_Stream6,ENABLE);
			}
			vTaskDelayUntil(&xLastWakeTime,1);

		}
	}
}



void uputc(USART_TypeDef* ch,char c){
	if(ch == USART2){
		if(xQueueSendToBackFromISR(usart2Que,&c,pdFALSE)!=pdPASS){
		}
	}
}

void prvRxTask(void *pvParameters){
	char* lineBuf = (char*)malloc(sizeof(char)*LINE_BUF_SIZE);
	int rxBufIndex=0;
	int lineBufIndex=0;
	
	for(int i=0;i<RX_BUFFERSIZE;i++){
		usart2RxBuf[i] = 0;
	}

	if((USART_TypeDef*)pvParameters == USART2){
		while(1){
			char c;
			while((c = usart2RxBuf[rxBufIndex])!=0){
				usart2RxBuf[rxBufIndex]=0;
				if(c=='\n'){
				}else if(c=='\r'){
					lineBuf[lineBufIndex]=0;
					
					callbackUsart2CrReceived(lineBuf);
					lineBufIndex=0;
					
				}else{
					lineBuf[lineBufIndex]=c;
					lineBufIndex++;
				}
				
				rxBufIndex=(rxBufIndex+1)%RX_BUFFERSIZE;
			}

			vTaskDelay(100);

		}
	}
}

void callbackUsart2CrReceived(char* line){
	handleSerialCommand(line);
}
