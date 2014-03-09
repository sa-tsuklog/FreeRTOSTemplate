/*
 * Ad7176-2.c
 *
 *  Created on: 2014/03/05
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
#include "AD7176-2/HalSpi2.h"
#include "Ad7176-2Seeker.h"
#include "Ad7176-2RegisterDef.h"
#include "Seeker.hpp"

unsigned char txBuf[8];
unsigned char rxBuf[8];

void write8(char cmd,char data){
	txBuf[0] = cmd;
	txBuf[1] = data;
	
	spi2ReadWrite(rxBuf,txBuf,2);
}

void write16(char cmd,unsigned short data){
	txBuf[0] = cmd;
	txBuf[1] = (unsigned char)(data>>8);
	txBuf[2] = (unsigned char)(data&0xFF);
	
	spi2ReadWrite(rxBuf,txBuf,3);
}
void write24(char cmd,int data){
	txBuf[0] = cmd;
	txBuf[1] = (unsigned char)(data>>16);
	txBuf[2] = (unsigned char)((data>>8)&0xFF);
	txBuf[3] = (unsigned char)(data&0xFF);
	
	spi2ReadWrite(rxBuf,txBuf,4);
}
unsigned char read8(char cmd){
	txBuf[0] = cmd|0x40;
	txBuf[1] = 0x00;
	
	spi2ReadWrite(rxBuf,txBuf,2);
	
	return rxBuf[1];
}
unsigned short read16(char cmd){
	txBuf[0] = cmd|0x40;
	txBuf[1] = 0x00;
	txBuf[2] = 0x00;
	
	spi2ReadWrite(rxBuf,txBuf,3);
	
	return ((unsigned short)rxBuf[1])<<8 | rxBuf[2];
}
unsigned int read24(char cmd){
	txBuf[0] = cmd|0x40;
	txBuf[1] = 0x00;
	txBuf[2] = 0x00;
	txBuf[3] = 0x00;
	
	spi2ReadWrite(rxBuf,txBuf,4);
	
	return ((unsigned int)rxBuf[1])<<16 | ((unsigned int)rxBuf[2]<<8) | rxBuf[3];
}

unsigned int read32(char cmd){
	txBuf[0] = cmd|0x40;
	txBuf[1] = 0x00;
	txBuf[2] = 0x00;
	txBuf[3] = 0x00;
	txBuf[4] = 0x00;
	
	spi2ReadWrite(rxBuf,txBuf,5);
	
	return ((unsigned int)rxBuf[1])<<24 | ((unsigned int)rxBuf[2]<<16) 
			|((unsigned int)rxBuf[3]<<8) | rxBuf[4];
}

void resetIf(){
	txBuf[0]=0xFF;
	txBuf[1]=0xFF;
	txBuf[2]=0xFF;
	txBuf[3]=0xFF;
	txBuf[4]=0xFF;
	txBuf[5]=0xFF;
	txBuf[6]=0xFF;
	txBuf[7]=0xFF;
	
	spi2ReadWrite(rxBuf,txBuf,8);
}

void initAd7176(){
	resetIf();
	
	vTaskDelay(1);
	waitForDataReady();
	write16(GPIOCON,0x080D);
	vTaskDelay(1);
	waitForDataReady();
	//write16(ADCMODE,0x800C);	//external clock
	waitForDataReady();
	write16(ADCMODE,0x8000);	//internal clock
	vTaskDelay(1);
	waitForDataReady();
	write16(IFMODE,0x0040);		//DATA_STAT ON
	
	vTaskDelay(1);
	waitForDataReady();
	write16(SETUPCON0,0x1020);
	vTaskDelay(1);
	waitForDataReady();
	write16(SETUPCON1,0x1020);
	vTaskDelay(1);
	waitForDataReady();
	write16(SETUPCON2,0x1020);
	vTaskDelay(1);
	waitForDataReady();
	write16(SETUPCON3,0x1020);
	vTaskDelay(1);
	waitForDataReady();
	write16(FILTCON0,0x0006);	//10ksps
	vTaskDelay(1);
	waitForDataReady();
	write16(FILTCON1,0x0007);	//10ksps
	vTaskDelay(1);
	waitForDataReady();
	write16(FILTCON2,0x0007);	//10ksps
	vTaskDelay(1);
	waitForDataReady();
	write16(FILTCON3,0x0007);	//10ksps
	vTaskDelay(1);
	waitForDataReady();
	write16(CHMAP0,0x8004);		//enable, AIN0 - AIN4
	vTaskDelay(1);
	waitForDataReady();
	write16(CHMAP1,0x0024);		//enable, AIN1 - AIN4
	vTaskDelay(1);
	waitForDataReady();
	write16(CHMAP2,0x0044);		//enable, AIN2 - AIN4
	vTaskDelay(1);
	waitForDataReady();
	write16(CHMAP3,0x0064);		//enable, AIN3 - AIN4
	
	unsigned short tmp = read16(CHMAP3);
	printf("%x\n\r",tmp);
}


void prvAd7176Task(void *pvParameters){
	void clearSemaphores();
	initAd7176();
	
	unsigned char ch=0;
	unsigned int readData=0;
	//float* buf = (float*)malloc(sizeof(float)*4);
//	if(buf == NULL){
//		printf("malloc error at prvAd7176Task\n\r");
//		while(1){}
//	}
	float signal;
	
	
	int i=0;
	while(1){
		waitForDataReady();
		readData = read32(DATA);
		ch = (unsigned char)(readData&0x03);
		signal= readData>>8;
		
//		if(i<16){
//			printf("%d %x\n\r",readData>>8,ch);
//		}
		
		i=(i+1)%1600;
		if(ch == 0){
			enqueAdData(signal);
		}
	}
}
