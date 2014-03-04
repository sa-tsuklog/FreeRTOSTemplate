/*
 * HalI2c2.cpp
 *
 *  Created on: 2014/02/23
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
#include "HalI2c2.h"
#include "Mpu9150RegisterDef.h"

#define RW_WRITE 0
#define RW_READN 1
#define RW_READ1 2
#define RW_READ_RS 3

#define STATE_TRANSMIT_DEVADDRESS 0
#define STATE_RECEIVE_DATA 1

int _rw;
int _i2cAddress;
int _state;

#define I2C2_BUFSIZE 16
char* i2c2TxBuf;
char* i2c2RxBuf;

xSemaphoreHandle i2c2Sem;


void initI2c2(){
	i2c2TxBuf = (char*)malloc(sizeof(char)*I2C2_BUFSIZE);
	i2c2RxBuf = (char*)malloc(sizeof(char)*I2C2_BUFSIZE);
	
	if(i2c2TxBuf == NULL || i2c2RxBuf == NULL){
		printf("malloc error at initI2c2.\n\r");
		while(1){}
	}
	
	vSemaphoreCreateBinary(i2c2Sem);
	xSemaphoreTake(i2c2Sem,1);
	
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
	dma1_2.DMA_Memory0BaseAddr = (uint32_t)i2c2RxBuf;
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
	dma1_7.DMA_Memory0BaseAddr = (uint32_t)i2c2TxBuf;
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
}

void prvI2C2SendTask(void *pvParameters){
	
	char* tmp = (char*)malloc(sizeof(char)*16);
	
	i2c2Write1(MPU9150ADDR,PWR_MGMT_1,0x80);
	i2c2Write1(MPU9150ADDR,SIGNAL_PATH_RESET,0x07);
	vTaskDelay(10);
	i2c2Write1(MPU9150ADDR,SIGNAL_PATH_RESET,0x00);
	
	i2c2Write1(MPU9150ADDR,PWR_MGMT_1,0x00);
	i2c2Write1(MPU9150ADDR,PWR_MGMT_2,0x00);
	
	i2c2Write1(MPU9150ADDR,SMPLRT_DIV,4); //1kHz
	i2c2Write1(MPU9150ADDR,CONFIG,0x01);
	i2c2Write1(MPU9150ADDR,FIFO_EN,0x00);
	
	i2c2Write1(MPU9150ADDR,I2C_MST_CTRL,0x00);
	i2c2Write1(MPU9150ADDR,INT_PIN_CFG,1<<1);
	
	i2c2Write1(MPU9150ADDR,GYRO_CONFIG,3<<3);
	i2c2Write1(MPU9150ADDR,ACCEL_CONFIG,3<<3);
	
	i2c2Write1(AK8975_ADDR,CNTL,0x0F);
	
	i2c2Write1(AK8975_ADDR,CNTL,0x01);
	
	
	while(1){
		tmp[0]=0xC3;
		tmp[1]=0xCC;
		tmp[2]=0xFF;
		tmp[3]=0x00;
		tmp[4]=0x00;
		tmp[5]=0x01;
		
		
		i2c2Read(MPU9150ADDR,ACCEL_XOUT_H,tmp,1);
		i2c2Read(MPU9150ADDR,ACCEL_XOUT_L,tmp+1,1);
		i2c2Read(MPU9150ADDR,ACCEL_YOUT_H,tmp+2,1);
		i2c2Read(MPU9150ADDR,ACCEL_YOUT_L,tmp+3,1);
		i2c2Read(MPU9150ADDR,ACCEL_ZOUT_H,tmp+4,1);
		i2c2Read(MPU9150ADDR,ACCEL_ZOUT_L,tmp+5,1);
		
		for(int i=0;i<6;i++){
			printf("%d:read data:%x\n\r",i,tmp[i]);
		}
		
		printf("\n\r");
		
		
		vTaskDelay(50);
	}
}

int i2c2Write(char i2cAddress, char regAddress, char* writeData, int writeLength){
	if(writeLength+1 > I2C2_BUFSIZE){
		return 0;
	}
	
	i2c2TxBuf[0] = regAddress;
	for(int i=0;i<writeLength;i++){
		i2c2TxBuf[i+1] = writeData[i];
	}
	
	_rw = RW_WRITE;
	_i2cAddress = i2cAddress;
	
	DMA_ClearITPendingBit(DMA1_Stream7,DMA_IT_TCIF7);
	DMA_SetCurrDataCounter(DMA1_Stream7,writeLength+1);
	
	DMA_Cmd(DMA1_Stream7,ENABLE);
	I2C_GenerateSTART(I2C2,ENABLE);
	
	if(pdTRUE!=xSemaphoreTake(i2c2Sem,portMAX_DELAY)){
		while(1){}
	}
	
	return 1;
}
int i2c2Write1(char i2cAddress,char regAddress, char writeData){
	return i2c2Write(i2cAddress,regAddress,&writeData,1);
}

int i2c2Read(char i2cAddress,char regAddress, char* readBuf, int readLength){
	if(readLength+1 > I2C2_BUFSIZE){
		return 0;
	}
	
	for(int i=0;i<readLength;i++){
		i2c2RxBuf[i] = 0;
	}
	
	i2c2TxBuf[0] = regAddress;
	
	if(readLength == 1){
		_rw = RW_READ1;
	}else{
		_rw = RW_READN;
	}
	_state = STATE_TRANSMIT_DEVADDRESS;
	_i2cAddress = i2cAddress;
	
	DMA_Cmd(DMA1_Stream7,DISABLE);
	DMA_Cmd(DMA1_Stream2,DISABLE);
	
	DMA_SetCurrDataCounter(DMA1_Stream7,1);
	DMA_ClearITPendingBit(DMA1_Stream7,DMA_IT_TCIF7);
	DMA_SetCurrDataCounter(DMA1_Stream2,readLength);
	DMA_ClearITPendingBit(DMA1_Stream2,DMA_IT_TCIF2);
	
	
	DMA_Cmd(DMA1_Stream7,ENABLE);
	DMA_Cmd(DMA1_Stream2,ENABLE);
	I2C_GenerateSTART(I2C2,ENABLE);
	
	if(pdTRUE!=xSemaphoreTake(i2c2Sem,portMAX_DELAY)){
		while(1){}
	}
	
	for(int i=0;i<readLength;i++){
		readBuf[i] = i2c2RxBuf[i];
	}
	
	return 1;
}

void myI2C2_EV_IRQ_Write(){
	if(I2C_GetITStatus(I2C2,I2C_IT_SB)!=RESET){
		I2C_ReadRegister(I2C2,I2C_Register_SR1);
		I2C_Send7bitAddress(I2C2,_i2cAddress,I2C_Direction_Transmitter);
	}
	if(I2C_GetITStatus(I2C2,I2C_IT_ADDR)!=RESET){
		I2C_ReadRegister(I2C2,I2C_Register_SR1);
		I2C_ReadRegister(I2C2,I2C_Register_SR2);
	}	
	if(I2C_GetITStatus(I2C2,I2C_IT_BTF)!=RESET){
		I2C_ReadRegister(I2C2,I2C_Register_DR);
		
		I2C_GenerateSTOP(I2C2,ENABLE);
		
		xSemaphoreGiveFromISR(i2c2Sem,pdTRUE);
		portEND_SWITCHING_ISR(pdTRUE);

	}
}
void myI2C2_EV_IRQ_ReadN(){
	if(I2C_GetITStatus(I2C2,I2C_IT_SB)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_SB);
		if(_state == STATE_TRANSMIT_DEVADDRESS){
			I2C_Send7bitAddress(I2C2,_i2cAddress,I2C_Direction_Transmitter);
		}else{
			I2C_Send7bitAddress(I2C2,_i2cAddress,I2C_Direction_Receiver);
		}
		
	}
	if(I2C_GetITStatus(I2C2,I2C_IT_ADDR)!=RESET){
		if(_state == STATE_RECEIVE_DATA){
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
		_state = STATE_RECEIVE_DATA;
		I2C_GenerateSTART(I2C2,ENABLE);
	}
}
void myI2C2_EV_IRQ_Read1(){
	if(I2C_GetITStatus(I2C2,I2C_IT_SB)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_SB);
		if(_state == STATE_TRANSMIT_DEVADDRESS){
			I2C_Send7bitAddress(I2C2,_i2cAddress,I2C_Direction_Transmitter);
		}else{
			I2C_Send7bitAddress(I2C2,_i2cAddress,I2C_Direction_Receiver);
		}
	}
	if(I2C_GetITStatus(I2C2,I2C_IT_ADDR)!=RESET){
		if(_state == STATE_RECEIVE_DATA){
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
		_state = STATE_RECEIVE_DATA;
		I2C_GenerateSTART(I2C2,ENABLE);
	}
}

void myI2C2_EV_IRQHandler(){
	if(_rw == RW_WRITE){
		myI2C2_EV_IRQ_Write();
	}else if(_rw == RW_READN){
		myI2C2_EV_IRQ_ReadN();
	}else if(_rw == RW_READ1){
		myI2C2_EV_IRQ_Read1();
	}
	
	if(I2C_GetITStatus(I2C2,I2C_IT_ADD10)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_ADD10);
	}		
	if(I2C_GetITStatus(I2C2,I2C_IT_STOPF)!=RESET){
		I2C_ClearITPendingBit(I2C2,I2C_IT_STOPF);
	}
	
}                /* I2C2 Event                   */
void myI2C2_ER_IRQHandler(){
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
		xSemaphoreGiveFromISR(i2c2Sem,pdTRUE);
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
}                /* I2C2 Error                   */

void myDMA1_Stream2_IRQHandler(){
	if(DMA_GetITStatus(DMA1_Stream2,DMA_IT_TCIF2)!=RESET){
		DMA_ClearITPendingBit(DMA1_Stream2,DMA_IT_TCIF2);
		I2C_GenerateSTOP(I2C2,ENABLE);
		xSemaphoreGiveFromISR(i2c2Sem,pdTRUE);
		portEND_SWITCHING_ISR(pdTRUE);
	}
}
void myDMA1_Stream7_IRQHandler(){
	if(DMA_GetITStatus(DMA1_Stream7,DMA_IT_TCIF7)!=RESET){
		DMA_ClearITPendingBit(DMA1_Stream7,DMA_IT_TCIF7);
	}
}
