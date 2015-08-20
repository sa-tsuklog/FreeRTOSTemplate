/*
 * SBusPropo.cpp
 *
 *  Created on: 2015/08/12
 *      Author: sa
 */
#include <pch.h>

#include "SBusPropo.h"
#include "Driver/USART1Propo.h"

SBusPropo::SBusPropo(){
	rxQueue = xQueueCreate(RX_BUFFERSIZE,sizeof(uint8_t));
	USART1ClassPropo::GetInstance();
}

void SBusPropo::SBusPropoTask(){
	vTaskDelay(MS_INITIAL_DELAY);
	
	int index = 0;
	int dataCount;
	int watchdog=0;
	while(1){
		vTaskDelay(4);
		dataCount = uxQueueMessagesWaiting(rxQueue);
		if(dataCount == 0){
			if(watchdog == 0){
				decodeMessage(lineBuffer,index);
				test(lineBuffer,index);
			}
			watchdog++;
			index = 0;
		}else{
			for(int i=0;i<dataCount;i++){
				uint8_t c; 
				xQueueReceive(rxQueue,&c,0);
				lineBuffer[index] = c;
				
				if(index < RX_BUFFERSIZE){
					index++;
				}
			}
			watchdog=0;
		}
	}
}

void SBusPropo::test(uint8_t* lineBuffer,int length){
	static int decimator=0;
	decimator = (decimator + 1)%10;
	
	if(length > 25){
		length = 25;
	}
	
	if(decimator == 0){
		for(int i=0;i<length;i++){
			printf("0x%02x, ",lineBuffer[i]);
		}
		//printf("\r\n");
		
		
		printf("//%04x, %04x, %04x, %04x, %04x, %04x\r\n",aileron,elevator,rudder,throttle,swB,swC);
	}
}

void SBusPropo::decodeMessage(uint8_t* lineBuffer, int length){
	if(length != 25 && length != 28){
		return;
	}
	if(lineBuffer[0] != 0x0f){
		return;
	}
	
	aileron = (lineBuffer[2] & 0x07)<<8 | (lineBuffer[1] & 0xFF);
	elevator= (lineBuffer[3] & 0x3F)<<5 | (lineBuffer[2] & 0xF8>>3);
	throttle= (lineBuffer[5] & 0x01)<<10 | (lineBuffer[4] & 0xFF)<<2 | (lineBuffer[3] & 0xC0) >>6;
	rudder  = (lineBuffer[6] & 0x0F)<<7 | (lineBuffer[5] & 0xFE) >>1;
	
	if((lineBuffer[7]&0x7E) < 0x30){
		swC = 2;
	}else if((lineBuffer[7]&0x7E) < 0x60){
		swC = 1;
	}else{
		swC = 0;
	}
	
	uint16_t tmp = (lineBuffer[9] & 0x3)<<9 | (lineBuffer[8] & 0xFF)<<1;
	if(tmp < 0x300){
		swB = 2;
	}else if(tmp < 0x600){
		swB = 1;
	}else{
		swB = 0;
	}
	
}


void SBusPropo::SBusPropoTaskEntry(void *pvParameters){
	SBusPropo::GetInstance()->SBusPropoTask();
}
void SBusPropo::initSBusPropo(){
	xTaskCreate(&SBusPropo::SBusPropoTaskEntry,"sbus",1024,NULL,3,NULL);
}



