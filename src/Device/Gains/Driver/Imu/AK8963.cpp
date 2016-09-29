/*
 * AK8963.cpp
 *
 *  Created on: 2016/09/28
 *      Author: sa
 */

#include "FreeRTOS.h"
#include "task.h"

#include "AK8963.h"
#include "AK8963Reg.h"
#include "ImuSpi2.h"

void Ak8963::write8(uint8_t address,uint8_t data){
	uint8_t txBuf[2];
	uint8_t dummy[2];
	
	txBuf[0] = address;
	txBuf[1] = data;
	
	ImuSpi2::GetInstance()->ReadWrite(ImuSpi2::CH_AK8963,dummy,txBuf,2);
}

uint8_t Ak8963::read8(uint8_t address){
	uint8_t txBuf[2];
	uint8_t rxBuf[2];
	
	txBuf[0] = address | 0x80;
	txBuf[1] = 0;
	
	ImuSpi2::GetInstance()->ReadWrite(ImuSpi2::CH_AK8963,rxBuf,txBuf,2);
	
	return rxBuf[1];
}

void Ak8963::read48(uint8_t address,uint8_t* buf){
	uint8_t txBuf[7];
	uint8_t rxBuf[7];
	
	txBuf[0] = address | 0x80;
	for(int i=1;i<7;i++){
		txBuf[i] = 0;
	}
	
	ImuSpi2::GetInstance()->ReadWrite(ImuSpi2::CH_AK8963,rxBuf,txBuf,7);
	
	for(int i=0;i<6;i++){
		buf[i] = rxBuf[i+1];
	}
}
void Ak8963::init(){
	if(read8(AK8963Reg::WIA)!=0x48){
		printf("AK8963 communication error\r\n");
		while(1);
	}
	write8(AK8963Reg::CNTL2,0x01);
	while(read8(AK8963Reg::CNTL2) != 0x00){
		vTaskDelay(1);
	}
	write8(AK8963Reg::I2CDIS,0x1B);
	
	write8(AK8963Reg::CNTL1,0x0F);
	asa[0] = read8(AK8963Reg::ASAX);
	asa[1] = read8(AK8963Reg::ASAY);
	asa[2] = read8(AK8963Reg::ASAZ);
	
	write8(AK8963Reg::CNTL1,0x16);	//TODO. 8Hz,100Hzでノイズ量に差があるか確認
}
void Ak8963::readUtCmps(float* buf){
	uint8_t rawBuf[6];
	read48(AK8963Reg::HXL,rawBuf);
	
	int16_t h[3];
	for(int i=0;i<3;i++){
		h[i] = (rawBuf[i*2]<<8) | (rawBuf[i*2+1]);
		buf[i] = h[i] * ((asa[i]-128)*0.5/128+1);
	}
}
