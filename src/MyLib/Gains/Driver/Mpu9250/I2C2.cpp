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
#include "MyLib/Util/Util.h"

I2C2Class::I2C2Class():
m_rw(-1),
m_address(-1),
m_state(-1)
{
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

	i2cdef.I2C_ClockSpeed = 400000;
	i2cdef.I2C_DutyCycle = I2C_DutyCycle_2;
	i2cdef.I2C_Ack = I2C_Ack_Enable;
	i2cdef.I2C_Mode = I2C_Mode_I2C;
	i2cdef.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

	I2C_Init(I2C2,&i2cdef);
	I2C_DMACmd(I2C2,ENABLE);
	I2C_Cmd(I2C2,ENABLE);

	I2C_ITConfig(I2C2,I2C_IT_EVT|I2C_IT_ERR,ENABLE);
	I2C_ITConfig(I2C2,I2C_IT_BUF,DISABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	
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
	dma1_2.DMA_Priority = DMA_Priority_VeryHigh;
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
	dma1_7.DMA_Priority = DMA_Priority_VeryHigh;
	dma1_7.DMA_Channel = DMA_Channel_7;
	DMA_Init(DMA1_Stream7,&dma1_7);
	DMA_ITConfig(DMA1_Stream7,DMA_IT_TC,ENABLE);



	NVIC_InitTypeDef nvicdef;
	nvicdef.NVIC_IRQChannel = I2C2_EV_IRQn;
	nvicdef.NVIC_IRQChannelCmd = ENABLE;
	nvicdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	nvicdef.NVIC_IRQChannelSubPriority = 0xFF;

	NVIC_Init(&nvicdef);

	NVIC_InitTypeDef er_nvicdef;
	er_nvicdef.NVIC_IRQChannel = I2C2_ER_IRQn;
	er_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	er_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	er_nvicdef.NVIC_IRQChannelSubPriority = 0xFF;

	NVIC_Init(&er_nvicdef);

	NVIC_InitTypeDef dma1_2_nvicdef;
	dma1_2_nvicdef.NVIC_IRQChannel = DMA1_Stream2_IRQn;
	dma1_2_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	dma1_2_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	dma1_2_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	NVIC_Init(&dma1_2_nvicdef);

	NVIC_InitTypeDef dma1_7_nvicdef;
	dma1_7_nvicdef.NVIC_IRQChannel = DMA1_Stream7_IRQn;
	dma1_7_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	dma1_7_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	dma1_7_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	NVIC_Init(&dma1_7_nvicdef);

	NVIC_SetPriority(I2C2_ER_IRQn,0xFF);
	NVIC_SetPriority(I2C2_EV_IRQn,0xFF);
	NVIC_SetPriority(DMA1_Stream2_IRQn,0xFF);
	NVIC_SetPriority(DMA1_Stream7_IRQn,0xFF);

	//Trigger
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef pb12def;
	GPIO_StructInit(&pb12def);
	pb12def.GPIO_Pin = GPIO_Pin_12;
	pb12def.GPIO_Mode = GPIO_Mode_IN;
	pb12def.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB,&pb12def);
	
	EXTI_InitTypeDef exti12def;
	EXTI_StructInit(&exti12def);
	exti12def.EXTI_Line = EXTI_Line12;
	exti12def.EXTI_Mode = EXTI_Mode_Interrupt;
	exti12def.EXTI_Trigger = EXTI_Trigger_Rising;
	exti12def.EXTI_LineCmd =ENABLE;
	EXTI_Init(&exti12def);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource12);
	
	NVIC_InitTypeDef exti12itdef;
	exti12itdef.NVIC_IRQChannel = EXTI15_10_IRQn;
	exti12itdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	exti12itdef.NVIC_IRQChannelSubPriority = 0xFF;
	exti12itdef.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&exti12itdef);
	NVIC_SetPriority(EXTI15_10_IRQn,0xFF);
	
	
	
	rwCompleteSem = xSemaphoreCreateBinary();
	if(rwCompleteSem == NULL){
		Util::GetInstance()->myFprintf(0,stdout,"malloc error at initI2c2\r\n");
		while(1){}
	}
	xSemaphoreTake(rwCompleteSem,0);
	
	dataReadySem = xSemaphoreCreateBinary();
	if(dataReadySem == NULL){
		printf("malloc error at initI2c2\r\n");
		while(1){}
	}
	xSemaphoreTake(dataReadySem,0);
	
	
}

void I2C2Class::start(){
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

int I2C2Class::write(char i2cAddress, char regAddress, unsigned char* writeData, int writeLength){
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
	
	if(pdTRUE!=xSemaphoreTake(rwCompleteSem,portMAX_DELAY)){
		while(1){}
	}
	vTaskDelay(1);

	return 1;
}

int I2C2Class::write1(char i2cAddress,char regAddress, unsigned char writeData){
	return write(i2cAddress,regAddress,&writeData,1);
}
int I2C2Class::read(char i2cAddress,char regAddress, unsigned char* readBuf, int readLength){
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
	if(rwCompleteSem != NULL){
		//Util::GetInstance()->myFprintf(0,stdout,"sem take\r\n");
		xSemaphoreTake(rwCompleteSem,portMAX_DELAY);
	}

	for(int i=0;i<readLength;i++){
		readBuf[i] = m_rxBuf[i];
	}

	return 1;

}

void I2C2Class::myEV_IRQ_Write(){
	//Util::GetInstance()->myFprintf(0,stdout,"write\r\n");
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
		
		
		BaseType_t isWoken;
		xSemaphoreGiveFromISR(rwCompleteSem,&isWoken);
		portEND_SWITCHING_ISR(isWoken);
	}
}

void I2C2Class::myEV_IRQ_ReadN(){
	//Util::GetInstance()->myFprintf(0,stdout,"readN\r\n");
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

void I2C2Class::myEV_IRQ_Read1(){
	if(I2C_GetITStatus(I2C2,I2C_IT_SB)!=RESET){
		//Util::GetInstance()->myFprintf(0,stdout,"SB\r\n");
		I2C_ClearITPendingBit(I2C2,I2C_IT_SB);
		if(m_state == STATE_TRANSMIT_DEVADDRESS){
			I2C_Send7bitAddress(I2C2,m_address,I2C_Direction_Transmitter);
		}else{
			I2C_Send7bitAddress(I2C2,m_address,I2C_Direction_Receiver);
		}
	}
	if(I2C_GetITStatus(I2C2,I2C_IT_ADDR)!=RESET){
		//Util::GetInstance()->myFprintf(0,stdout,"ADDR\r\n");
		if(m_state == STATE_RECEIVE_DATA){
			I2C_AcknowledgeConfig(I2C2,DISABLE);
			I2C_ReadRegister(I2C2,I2C_Register_SR1);
			I2C_ReadRegister(I2C2,I2C_Register_SR2);
		}else{
			I2C_ReadRegister(I2C2,I2C_Register_SR1);
			I2C_ReadRegister(I2C2,I2C_Register_SR2);
		}
	}
	if(I2C_GetITStatus(I2C2,I2C_IT_BTF)!=RESET){
		//Util::GetInstance()->myFprintf(0,stdout,"BTF\r\n");
		I2C_ReadRegister(I2C2,I2C_Register_DR);
		m_state = STATE_RECEIVE_DATA;
		I2C_GenerateSTART(I2C2,ENABLE);
	}
}

void I2C2Class::myEV_IRQHandler(){
	if(m_rw == RW_WRITE){
		myEV_IRQ_Write();
	}else if(m_rw == RW_READN){
		myEV_IRQ_ReadN();
	}else if(m_rw == RW_READ1){
		myEV_IRQ_Read1();
	}

	if(I2C_GetITStatus(I2C2,I2C_IT_ADD10)!=RESET){
		Util::GetInstance()->myFprintf(0,stdout,"addr10\r\n");
		I2C_ClearITPendingBit(I2C2,I2C_IT_ADD10);
	}
	if(I2C_GetITStatus(I2C2,I2C_IT_STOPF)!=RESET){
		Util::GetInstance()->myFprintf(0,stdout,"STPOF\r\n");
		I2C_ClearITPendingBit(I2C2,I2C_IT_STOPF);
	}
}

void I2C2Class::myER_IRQHandler(){
	Util::GetInstance()->myFprintf(0,stdout,"err\r\n");
	if(I2C_GetITStatus(I2C2,I2C_IT_BERR)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_BERR);

		Util::GetInstance()->myFprintf(0,stdout,"berr\r\n");

	}else if(I2C_GetITStatus(I2C2,I2C_IT_ARLO)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_ARLO);

		Util::GetInstance()->myFprintf(0,stdout,"arlo\r\n");

	}else if(I2C_GetITStatus(I2C2,I2C_IT_AF)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_AF);
		I2C_GenerateSTOP(I2C2,ENABLE);
		Util::GetInstance()->myFprintf(0,stdout,"af\r\n");
		portBASE_TYPE isWoken;
		xSemaphoreGiveFromISR(rwCompleteSem,&isWoken);
		portEND_SWITCHING_ISR(isWoken);


	}else if(I2C_GetITStatus(I2C2,I2C_IT_OVR)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_OVR);
		Util::GetInstance()->myFprintf(0,stdout,"ovr\r\n");
	}else if(I2C_GetITStatus(I2C2,I2C_IT_PECERR)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_PECERR);
		Util::GetInstance()->myFprintf(0,stdout,"pecerr\r\n");
	}else if(I2C_GetITStatus(I2C2,I2C_IT_TIMEOUT)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_TIMEOUT);
		Util::GetInstance()->myFprintf(0,stdout,"timeout\r\n");
	}else if(I2C_GetITStatus(I2C2,I2C_IT_SMBALERT)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_SMBALERT);
		Util::GetInstance()->myFprintf(0,stdout,"smbalert\r\n");
	}
}

void I2C2Class::myDMA1_Stream2_IRQHandler(){
	if(DMA_GetITStatus(DMA1_Stream2,DMA_IT_TCIF2)!=RESET){
		DMA_ClearITPendingBit(DMA1_Stream2,DMA_IT_TCIF2);
		I2C_GenerateSTOP(I2C2,ENABLE);
		
		BaseType_t isWoken;
		xSemaphoreGiveFromISR(rwCompleteSem,&isWoken);
		portEND_SWITCHING_ISR(isWoken);
		//Util::GetInstance()->myFprintf(0,stdout,"sem give\r\n");
	}
}

void I2C2Class::myDMA1_Stream7_IRQHandler(){
	if(DMA_GetITStatus(DMA1_Stream7,DMA_IT_TCIF7)!=RESET){
		DMA_ClearITPendingBit(DMA1_Stream7,DMA_IT_TCIF7);
	}
}

void I2C2Class::waitNewData(){
	xSemaphoreTake(dataReadySem,portMAX_DELAY);
}

void I2C2Class::myEXTI12IRQHandler(){
	portBASE_TYPE isSWitchRequired;
	xSemaphoreGiveFromISR(dataReadySem,&isSWitchRequired);
	portEND_SWITCHING_ISR(isSWitchRequired);
}
