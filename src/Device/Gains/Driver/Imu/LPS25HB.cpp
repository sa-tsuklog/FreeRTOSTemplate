/*
 * LPS25HB.cpp
 *
 *  Created on: 2016/09/28
 *      Author: sa
 */

#include "FreeRTOS.h"
#include "task.h"

#include "LPS25HB.h"
#include "LPS25HBReg.h"
#include "ImuSpi2.h"

void Lps25Hb::write8(uint8_t address, uint8_t data){
	uint8_t txBuf[2];
	uint8_t dummy[2];
	
	txBuf[0] = address;
	txBuf[1] = data;
	
	ImuSpi2::GetInstance()->ReadWrite(ImuSpi2::CH_LPS25HB,dummy,txBuf,2);
}

uint32_t Lps25Hb::read24(uint8_t address){
	uint8_t txBuf[4];
	uint8_t rxBuf[4];
	
	txBuf[0] = address | 0xC0;
	txBuf[1] = 0;
	txBuf[2] = 0;
	txBuf[3] = 0;
	
	ImuSpi2::GetInstance()->ReadWrite(ImuSpi2::CH_LPS25HB,rxBuf,txBuf,4);
	
	return (rxBuf[3]<<16) | (rxBuf[2]<<8) | rxBuf[1];
}

uint16_t Lps25Hb::read16(uint8_t address){
	uint8_t txBuf[3];
	uint8_t rxBuf[3];
	
	txBuf[0] = address | 0xC0;
	txBuf[1] = 0;
	txBuf[2] = 0;
	
	ImuSpi2::GetInstance()->ReadWrite(ImuSpi2::CH_LPS25HB,rxBuf,txBuf,3);
	
	return (rxBuf[2]<<8) | rxBuf[1];
}

uint8_t Lps25Hb::read8(uint8_t address){
	uint8_t txBuf[2];
	uint8_t rxBuf[2];
	
	txBuf[0] = address | 0xC0;
	txBuf[1] = 0;
	
	ImuSpi2::GetInstance()->ReadWrite(ImuSpi2::CH_LPS25HB,rxBuf,txBuf,2);
	
	return rxBuf[1];
}

void Lps25Hb::init(){
	if(read8(Lps25HbReg::WHO_AM_I) != 0xBD){
		printf("LPS25HB communication error\r\n");
		while(1);
	}
	
	write8(Lps25HbReg::CTRL_REG2,0x04);
	vTaskDelay(1);
	while(read8(Lps25HbReg::CTRL_REG2) != 0x00){
		vTaskDelay(1);
	}
	
	write8(Lps25HbReg::CTRL_REG2,0x80);
	vTaskDelay(3);
	while(read8(Lps25HbReg::CTRL_REG2) != 0x00){
		vTaskDelay(3);
	}
	
	write8(Lps25HbReg::CTRL_REG2,0x08);
	write8(Lps25HbReg::CTRL_REG3,0x00);
	write8(Lps25HbReg::CTRL_REG4,0x01);
	write8(Lps25HbReg::FIFO_CTRL,0x00);
	
	write8(Lps25HbReg::CTRL_REG1,0xC4);
}
float Lps25Hb::readPaPressure(){
	int32_t rawPrs = read16(Lps25HbReg::PRESS_OUT_XL);
	if(rawPrs & 0x00800000){
		rawPrs |= 0xFF000000;
	}
	return ((float)rawPrs)/4096.0*100.0;
}

float Lps25Hb::readDegTemp(){
	int16_t rawTemp = read16(Lps25HbReg::TMP_OUT_L);
	
	return ((float)rawTemp)/480.0;
	
}
