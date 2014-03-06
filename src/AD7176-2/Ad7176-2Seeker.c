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

unsigned char txBuf[5];
unsigned char rxBuf[5];

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
	
	spi2ReadWrite(rxBuf,txBuf,4);
	
	return ((unsigned int)rxBuf[1])<<24 | ((unsigned int)rxBuf[2]<<16) 
			|((unsigned int)rxBuf[3]<<8) | rxBuf[4];
}

void initAd7176(){
	write16(GPIOCON,0x000D);
	
	write16(IFMODE,0x0040);		//DATA_STAT ON
	
	write16(SETUPCON0,0x1000);
	
	write16(FILTCON0,0x0005);	//25ksps
	
	write16(CHMAP0,0x8004);		//enable, AIN0 - AIN4
	//write16(CHMAP1,0x8024);		//enable, AIN1 - AIN4
	//write16(CHMAP2,0x8044);		//enable, AIN2 - AIN4
	//write16(CHMAP3,0x8064);		//enable, AIN3 - AIN4
	
	write16(ADCMODE,0x8003);
}


void prvAd7176Task(void *pvParameters){
	initAd7176();
	
	int loop = 128;
	
	int* buf = (int*)malloc(sizeof(int)*loop);
	
	while(1){
		for(int i=0;i<loop;i++){
			waitForDataReady();
			buf[i] = read24(DATA);
		}
		
		for(int i=0;i<loop;i++){
			printf("%d %d\n\r",i,buf[i]);
			vTaskDelay(1);
		}
		
		vTaskDelay(1000);
	}
}
