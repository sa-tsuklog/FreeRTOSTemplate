#include "GeneralConfig.h"
#include "Gps.h"
#include "USART2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"


USART2Class::USART2Class(){
	m_queue = xQueueCreate(TX_BUFFERSIZE,sizeof(char));
	vQueueAddToRegistry(m_queue,"u2tx");
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

	GPIO_InitTypeDef pd5def;

	GPIO_StructInit(&pd5def);
	pd5def.GPIO_Pin = GPIO_Pin_5;
	pd5def.GPIO_Mode = GPIO_Mode_AF;
	pd5def.GPIO_OType = GPIO_OType_PP;
	pd5def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pd5def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD,&pd5def);

	GPIO_InitTypeDef pd6def;
	GPIO_StructInit(&pd6def);
	pd6def.GPIO_Pin = GPIO_Pin_6;
	pd6def.GPIO_Mode = GPIO_Mode_AF;
	pd6def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pd6def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD,&pd6def);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);


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
	dma1_6.DMA_Memory0BaseAddr = (uint32_t)m_txBuf;
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
	dma1_5.DMA_Memory0BaseAddr = (uint32_t)m_rxBuf;
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


	for(int i=0;i<RX_BUFFERSIZE;i++){
		m_rxBuf[i] = 0;
	}
}

void USART2Class::Tx()
{
	portTickType xLastWakeTime = xTaskGetTickCount();
	vTaskDelayUntil(&xLastWakeTime,MS_INITIAL_DELAY);
	while(1){
		int numTx;
		if(DMA_GetCmdStatus(DMA1_Stream6)==DISABLE && (numTx = uxQueueMessagesWaiting(m_queue)) != 0){
			for(int i=0;i<numTx;i++){
				char c;
				xQueueReceive(m_queue,&c,0);
				m_txBuf[i] = c;
			}

			DMA_SetCurrDataCounter(DMA1_Stream6,numTx);
			USART_ClearFlag(USART2,USART_SR_TC);
			DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_TCIF6);
			DMA_Cmd(DMA1_Stream6,ENABLE);
		}
		
		vTaskDelayUntil(&xLastWakeTime,1);
	}
}

void USART2Class::Rx()
{
	int rxBufIndex = 0;
	int lineBufIndex = 0;

	char c;
	vTaskDelay(MS_INITIAL_DELAY);
	while(1){
		while( (c = m_rxBuf[rxBufIndex]) != 0 ){
			m_rxBuf[rxBufIndex]=0;
			if(c=='\r'){
			}else if(c=='\n'){
				m_lineBuf[lineBufIndex]=0;
	
				//HandleSerialCommand(m_lineBuf);
				Gps::GetInstance()->decodeNMEA((char*)m_lineBuf);
				
				lineBufIndex=0;
				
			}else{
				m_lineBuf[lineBufIndex]=c;
				lineBufIndex++;
			}
	
			rxBufIndex=(rxBufIndex+1)%RX_BUFFERSIZE;
		}
		vTaskDelay(1);
	}
}

void USART2Class::prvTxTask(void *pvParameters)
{
	USART2Class::GetInstance()->Tx();
}

void USART2Class::prvRxTask(void *pvParameters)
{
	USART2Class::GetInstance()->Rx();
}
