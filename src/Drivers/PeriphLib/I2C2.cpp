/*
 * I2C2.cpp
 *
 *  Created on: 2014/03/14
 *      Author: Lark_mp
 */

#include "stdio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "I2C2.h"
#include "task.h"

I2C2Class::I2C2Class(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	GPIO_InitTypeDef pb10_11def;
	GPIO_StructInit(&pb10_11def);
	pb10_11def.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	pb10_11def.GPIO_Mode = GPIO_Mode_AF;
	pb10_11def.GPIO_OType = GPIO_OType_OD;
	pb10_11def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pb10_11def.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOB,&pb10_11def);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_I2C2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_I2C2);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);

	I2C_InitTypeDef i2cdef;

	I2C_StructInit(&i2cdef);

	i2cdef.I2C_ClockSpeed = 100000;
	i2cdef.I2C_DutyCycle = I2C_DutyCycle_2;
	i2cdef.I2C_Ack = I2C_Ack_Enable;
	i2cdef.I2C_Mode = I2C_Mode_I2C;
	i2cdef.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

	I2C_Init(I2C2,&i2cdef);
	I2C_DMACmd(I2C2,ENABLE);
	I2C_Cmd(I2C2,ENABLE);

	I2C_ITConfig(I2C2,I2C_IT_EVT|I2C_IT_ERR,ENABLE);
	I2C_ITConfig(I2C2,I2C_IT_BUF,DISABLE);

	DMA_InitTypeDef dma1_2;
	DMA_StructInit(&dma1_2);
	dma1_2.DMA_PeripheralBaseAddr = (uint32_t)&(I2C2->DR);
	dma1_2.DMA_Memory0BaseAddr = (uint32_t)m_rxBuf;
	dma1_2.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma1_2.DMA_BufferSize = I2C2_BUFSIZE;
	dma1_2.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_2.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_2.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma1_2.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma1_2.DMA_Mode = DMA_Mode_Normal;
	dma1_2.DMA_Priority = DMA_Priority_Medium;
	dma1_2.DMA_Channel = DMA_Channel_7;
	DMA_Init(DMA1_Stream2,&dma1_2);
	DMA_ITConfig(DMA1_Stream2,DMA_IT_TC,ENABLE);

	DMA_InitTypeDef dma1_7;
	DMA_StructInit(&dma1_7);
	dma1_7.DMA_PeripheralBaseAddr = (uint32_t)&(I2C2->DR);
	dma1_7.DMA_Memory0BaseAddr = (uint32_t)m_txBuf;
	dma1_7.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma1_7.DMA_BufferSize = I2C2_BUFSIZE;
	dma1_7.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma1_7.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma1_7.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma1_7.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma1_7.DMA_Mode = DMA_Mode_Normal;
	dma1_7.DMA_Priority = DMA_Priority_Medium;
	dma1_7.DMA_Channel = DMA_Channel_7;
	DMA_Init(DMA1_Stream7,&dma1_7);
	DMA_ITConfig(DMA1_Stream7,DMA_IT_TC,ENABLE);



	NVIC_InitTypeDef nvicdef;
	nvicdef.NVIC_IRQChannel = I2C2_EV_IRQn;
	nvicdef.NVIC_IRQChannelCmd = ENABLE;
	nvicdef.NVIC_IRQChannelPreemptionPriority = 0xF;
	nvicdef.NVIC_IRQChannelSubPriority = 0xF;

	NVIC_Init(&nvicdef);

	NVIC_InitTypeDef er_nvicdef;
	er_nvicdef.NVIC_IRQChannel = I2C2_ER_IRQn;
	er_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	er_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xF;
	er_nvicdef.NVIC_IRQChannelSubPriority = 0xF;

	NVIC_Init(&er_nvicdef);

	NVIC_InitTypeDef dma1_2_nvicdef;
	dma1_2_nvicdef.NVIC_IRQChannel = DMA1_Stream2_IRQn;
	dma1_2_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	dma1_2_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xF;
	dma1_2_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xF;
	NVIC_Init(&dma1_2_nvicdef);

	NVIC_InitTypeDef dma1_7_nvicdef;
	dma1_7_nvicdef.NVIC_IRQChannel = DMA1_Stream7_IRQn;
	dma1_7_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	dma1_7_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xF;
	dma1_7_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xF;
	NVIC_Init(&dma1_7_nvicdef);

	NVIC_SetPriority(I2C2_ER_IRQn,0xFF);
	NVIC_SetPriority(I2C2_EV_IRQn,0xFF);
	NVIC_SetPriority(DMA1_Stream2_IRQn,0xFF);
	NVIC_SetPriority(DMA1_Stream7_IRQn,0xFF);

	m_sem = xSemaphoreCreateBinary();
	if(m_sem == NULL){
		printf("malloc error at initI2c2\n\r");
		while(1){}
	}
	xSemaphoreTake(m_sem,1);
}


int I2C2Class::Write(char i2cAddress, char regAddress, char* writeData, int writeLength){
	if(writeLength+1 > I2C2_BUFSIZE){
		return 0;
	}

	m_txBuf[0] = regAddress;
	for(int i=0;i<writeLength;i++){
		m_txBuf[i+1] = writeData[i];
	}

	m_rw = RW_WRITE;
	m_address = i2cAddress;

	DMA_ClearITPendingBit(DMA1_Stream7,DMA_IT_TCIF7);
	DMA_SetCurrDataCounter(DMA1_Stream7,writeLength+1);

	DMA_Cmd(DMA1_Stream7,ENABLE);
	I2C_GenerateSTART(I2C2,ENABLE);

	if(pdTRUE!=xSemaphoreTake(m_sem,portMAX_DELAY)){
		while(1){}
	}

	return 1;
}

int I2C2Class::Write1(char i2cAddress,char regAddress, char writeData){
	return Write(i2cAddress,regAddress,&writeData,1);
}
int I2C2Class::Read(char i2cAddress,char regAddress, char* readBuf, int readLength){
	if(readLength+1 > I2C2_BUFSIZE){
		return 0;
	}

	for(int i=0;i<readLength;i++){
		m_rxBuf[i] = 0;
	}

	m_txBuf[0] = regAddress;

	if(readLength == 1){
		m_rw = RW_READ1;
	}else{
		m_rw = RW_READN;
	}
	m_state = STATE_TRANSMIT_DEVADDRESS;
	m_address = i2cAddress;

	DMA_Cmd(DMA1_Stream7,DISABLE);
	DMA_Cmd(DMA1_Stream2,DISABLE);

	DMA_SetCurrDataCounter(DMA1_Stream7,1);
	DMA_ClearITPendingBit(DMA1_Stream7,DMA_IT_TCIF7);
	DMA_SetCurrDataCounter(DMA1_Stream2,readLength);
	DMA_ClearITPendingBit(DMA1_Stream2,DMA_IT_TCIF2);


	DMA_Cmd(DMA1_Stream7,ENABLE);
	DMA_Cmd(DMA1_Stream2,ENABLE);
	I2C_GenerateSTART(I2C2,ENABLE);

	if(pdTRUE!=xSemaphoreTake(m_sem,portMAX_DELAY)){
		while(1){}
	}

	for(int i=0;i<readLength;i++){
		readBuf[i] = m_rxBuf[i];
	}

	return 1;

}

void I2C2Class::EV_IRQ_Write(){
	if(I2C_GetITStatus(I2C2,I2C_IT_SB)!=RESET){
		I2C_ReadRegister(I2C2,I2C_Register_SR1);
		I2C_Send7bitAddress(I2C2,m_address,I2C_Direction_Transmitter);
	}
	if(I2C_GetITStatus(I2C2,I2C_IT_ADDR)!=RESET){
		I2C_ReadRegister(I2C2,I2C_Register_SR1);
		I2C_ReadRegister(I2C2,I2C_Register_SR2);
	}
	if(I2C_GetITStatus(I2C2,I2C_IT_BTF)!=RESET){
		I2C_ReadRegister(I2C2,I2C_Register_DR);

		I2C_GenerateSTOP(I2C2,ENABLE);

		xSemaphoreGiveFromISR(m_sem,(BaseType_t *)pdTRUE);
		portEND_SWITCHING_ISR(pdTRUE);

	}
}

void I2C2Class::EV_IRQ_ReadN(){
	if(I2C_GetITStatus(I2C2,I2C_IT_SB)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_SB);
		if(m_state == STATE_TRANSMIT_DEVADDRESS){
			I2C_Send7bitAddress(I2C2,m_address,I2C_Direction_Transmitter);
		}else{
			I2C_Send7bitAddress(I2C2,m_address,I2C_Direction_Receiver);
		}

	}
	if(I2C_GetITStatus(I2C2,I2C_IT_ADDR)!=RESET){
		if(m_state == STATE_RECEIVE_DATA){
			I2C_DMALastTransferCmd(I2C2,ENABLE);
			I2C_ReadRegister(I2C2,I2C_Register_SR1);
			I2C_ReadRegister(I2C2,I2C_Register_SR2);
		}else{
			I2C_DMALastTransferCmd(I2C2,DISABLE);
			I2C_ReadRegister(I2C2,I2C_Register_SR1);
			I2C_ReadRegister(I2C2,I2C_Register_SR2);
		}

	}
	if(I2C_GetITStatus(I2C2,I2C_IT_BTF)!=RESET){
		I2C_ReadRegister(I2C2,I2C_Register_DR);
		m_state = STATE_RECEIVE_DATA;
		I2C_GenerateSTART(I2C2,ENABLE);
	}
}

void I2C2Class::EV_IRQ_Read1(){
	if(I2C_GetITStatus(I2C2,I2C_IT_SB)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_SB);
		if(m_state == STATE_TRANSMIT_DEVADDRESS){
			I2C_Send7bitAddress(I2C2,m_address,I2C_Direction_Transmitter);
		}else{
			I2C_Send7bitAddress(I2C2,m_address,I2C_Direction_Receiver);
		}
	}
	if(I2C_GetITStatus(I2C2,I2C_IT_ADDR)!=RESET){
		if(m_state == STATE_RECEIVE_DATA){
			I2C_AcknowledgeConfig(I2C2,DISABLE);
			//I2C_DMALastTransferCmd(I2C2,ENABLE);
			I2C_ReadRegister(I2C2,I2C_Register_SR1);
			I2C_ReadRegister(I2C2,I2C_Register_SR2);

			I2C_GenerateSTOP(I2C2,ENABLE);
		}else{
			I2C_ReadRegister(I2C2,I2C_Register_SR1);
			I2C_ReadRegister(I2C2,I2C_Register_SR2);
		}
	}
	if(I2C_GetITStatus(I2C2,I2C_IT_BTF)!=RESET){
		I2C_ReadRegister(I2C2,I2C_Register_DR);
		m_state = STATE_RECEIVE_DATA;
		I2C_GenerateSTART(I2C2,ENABLE);
	}
}

void I2C2Class::EV_IRQHandler(){
	if(m_rw == RW_WRITE){
		EV_IRQ_Write();
	}else if(m_rw == RW_READN){
		EV_IRQ_ReadN();
	}else if(m_rw == RW_READ1){
		EV_IRQ_Read1();
	}

	if(I2C_GetITStatus(I2C2,I2C_IT_ADD10)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_ADD10);
	}
	if(I2C_GetITStatus(I2C2,I2C_IT_STOPF)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_STOPF);
	}
}

void I2C2Class::ER_IRQHandler(){
	if(I2C_GetITStatus(I2C2,I2C_IT_BERR)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_BERR);

		printf("berr\n\r");

	}else if(I2C_GetITStatus(I2C2,I2C_IT_ARLO)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_ARLO);

		printf("arlo\n\r");

	}else if(I2C_GetITStatus(I2C2,I2C_IT_AF)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_AF);
		I2C_GenerateSTOP(I2C2,ENABLE);
		printf("af\n\r");
		xSemaphoreGiveFromISR(m_sem,(BaseType_t *)pdTRUE);
		portEND_SWITCHING_ISR(pdTRUE);


	}else if(I2C_GetITStatus(I2C2,I2C_IT_OVR)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_OVR);
		printf("ovr\n\r");
	}else if(I2C_GetITStatus(I2C2,I2C_IT_PECERR)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_PECERR);
		printf("pecerr\n\r");
	}else if(I2C_GetITStatus(I2C2,I2C_IT_TIMEOUT)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_TIMEOUT);
		printf("timeout\n\r");
	}else if(I2C_GetITStatus(I2C2,I2C_IT_SMBALERT)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_SMBALERT);
		printf("smbalert\n\r");
	}
}

void I2C2Class::DMA1_Stream2_IRQHandler(){
	if(DMA_GetITStatus(DMA1_Stream2,DMA_IT_TCIF2)!=RESET){
		DMA_ClearITPendingBit(DMA1_Stream2,DMA_IT_TCIF2);
		I2C_GenerateSTOP(I2C2,ENABLE);
		xSemaphoreGiveFromISR(m_sem,(BaseType_t *)pdTRUE);
		portEND_SWITCHING_ISR(pdTRUE);
	}
}

void I2C2Class::DMA1_Stream7_IRQHandler(){
	if(DMA_GetITStatus(DMA1_Stream7,DMA_IT_TCIF7)!=RESET){
		DMA_ClearITPendingBit(DMA1_Stream7,DMA_IT_TCIF7);
	}
}
