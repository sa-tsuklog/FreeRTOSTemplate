/*
 * FpgaSeeker.cpp
 *
 *  Created on: 2016/09/22
 *      Author: sa
 */


#include "FreeRTOS.h"
#include "task.h"

#include "FpgaSeeker.h"
#include "Driver/FpgaSPI4.h"

FpgaSeeker::FpgaSeeker(){
	
}

void FpgaSeeker::write32(uint8_t address,uint32_t value){
	uint8_t writeBuf[5];
	uint8_t dummy[5];
	
	writeBuf[0] = address | 0x80;
	writeBuf[1] = ((value&0xFF000000)>>24);
	writeBuf[2] = ((value&0x00FF0000)>>16);
	writeBuf[3] = ((value&0x0000FF00)>>8);
	writeBuf[4] = ((value&0x000000FF)>>0);
	
	FpgaSPI4Class::GetInstance()->ReadWrite(dummy,writeBuf,5);
}
uint32_t FpgaSeeker::read32(uint8_t address){
	uint8_t writeBuf[5];
	uint8_t readBuf[5];
	
	writeBuf[0] = address;
	writeBuf[1] = 0x00;
	writeBuf[2] = 0x00;
	writeBuf[3] = 0x00;
	writeBuf[4] = 0x00;
	
	FpgaSPI4Class::GetInstance()->ReadWrite(readBuf,writeBuf,5);
	
	uint32_t rt;
	rt = readBuf[1]<<24 | readBuf[2]<<16 | readBuf[3]<<8 | readBuf[4];
	
	return rt;
}

void FpgaSeeker::FpgaSeekerTask(){
	
	int32_t ch0Val;
	int32_t ch1Val;
	int32_t ch2Val;
	int32_t ch3Val;
	
	while(1){
		ch0Val = read32(ADDR_CH0);
		ch1Val = read32(ADDR_CH1);
		ch2Val = read32(ADDR_CH2);
		ch3Val = read32(ADDR_CH3);
		
		//printf("%8d, %8d, %8d, %8d\r\n",ch0Val,ch1Val,ch2Val,ch3Val);
		//printf("%08x, %08x, %08x, %08x\r\n",ch0Val,ch1Val,ch2Val,ch3Val);
		intensity = (0.0f + ch0Val + ch1Val + ch2Val + ch3Val)/2;
		upDown = (1.0f + ch0Val - ch1Val)/(ch0Val + ch1Val);
		leftRight = (1.0f + ch2Val -ch3Val)/(ch2Val + ch3Val);
		printf("%1.3f,\t%1.3f,\t%8f\r\n",upDown,leftRight,intensity);
		
		vTaskDelay(100);
	}
}

void FpgaSeeker::FpgaSeekerTaskEntry(void *pvParameters){
	FpgaSeeker::GetInstance()->FpgaSeekerTask();
}
void FpgaSeeker::initFpgaSeeker(){
	xTaskCreate(&FpgaSeeker::FpgaSeekerTaskEntry,"fskr",2048,NULL,3,NULL);
}


