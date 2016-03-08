/*
 * Bmp085.cpp
 *
 *  Created on: 2015/06/17
 *      Author: sa
 */
#include <pch.h>

#include "I2C2.h"
#include "Bmp085.h"
#include "Bmp085RegisterDefs.h"

Bmp085::Bmp085(){
}

void Bmp085::init(){
	uint8_t buf[2];
	I2C2Class::getInstance()->read(BMP085ADDR,AC1,buf,2);
	ac1 = (((int16_t)buf[0])<<8)|((int16_t)buf[1]);
	I2C2Class::getInstance()->read(BMP085ADDR,AC2,buf,2);
	ac2 = (((int16_t)buf[0])<<8)|((int16_t)buf[1]);
	I2C2Class::getInstance()->read(BMP085ADDR,AC3,buf,2);
	ac3 = (((int16_t)buf[0])<<8)|((int16_t)buf[1]);
	I2C2Class::getInstance()->read(BMP085ADDR,AC4,buf,2);
	ac4 = (((uint16_t)buf[0])<<8)|((int16_t)buf[1]);
	I2C2Class::getInstance()->read(BMP085ADDR,AC5,buf,2);
	ac5 = (((uint16_t)buf[0])<<8)|((uint16_t)buf[1]);
	I2C2Class::getInstance()->read(BMP085ADDR,AC6,buf,2);
	ac6 = (((uint16_t)buf[0])<<8)|((uint16_t)buf[1]);
	I2C2Class::getInstance()->read(BMP085ADDR,B1,buf,2);
	b1 = (((int16_t)buf[0])<<8)|((int16_t)buf[1]);
	I2C2Class::getInstance()->read(BMP085ADDR,B2,buf,2);
	b2 = (((int16_t)buf[0])<<8)|((int16_t)buf[1]);
	I2C2Class::getInstance()->read(BMP085ADDR,MB,buf,2);
	mb = (((int16_t)buf[0])<<8)|((int16_t)buf[1]);
	I2C2Class::getInstance()->read(BMP085ADDR,MC,buf,2);
	mc = (((int16_t)buf[0])<<8)|((int16_t)buf[1]);
	I2C2Class::getInstance()->read(BMP085ADDR,MD,buf,2);
	md = (((int16_t)buf[0])<<8)|((int16_t)buf[1]);
	
	for(int i=0;i<10;i++){
		startMeasureingTemp();
		vTaskDelay(10);
		updateTemp();
		
		startMeasureingPressure();
		vTaskDelay(10);
		updatePressure();
	}
	
	startMeasureingTemp();
}
void Bmp085::startMeasureingTemp(){
	I2C2Class::getInstance()->write1(BMP085ADDR,SR,0x2E);
}
void Bmp085::startMeasureingPressure(){
	I2C2Class::getInstance()->write1(BMP085ADDR,SR,0x34);
}
void Bmp085::updatePressure(){
	uint8_t buf[3];
	I2C2Class::getInstance()->read(BMP085ADDR,RAW_PRS,buf,3);
	up = ((((int32_t)buf[0])<<16)|(((int32_t)buf[1])<<8)|(((int32_t)buf[2])<<0))>>8;
	
	int b6 = b5-4000;
	int x1 = (b2*(b6*b6/(2<<11)))/(2<<10);
	int x2 = ac2 * b6/(2<<10);
	int x3 = x1+x2;
	int b3 = ((ac1*4+x3)+2)/4;
	x1 = ac3*b6/(2<<12);
	x2 = (b1*(b6*b6/(2<<11)))/(2<<15);
	x3 = ((x1+x2)+2)/(2<<1);
	unsigned int b4 = ac4*(x3+32768)/(2<<14);
	unsigned int b7 = (up -b3) * (50000);
	
	int p;
	if(b7 < 0x80000000){
		p = b7*2/b4;
	}else{
		p = (b7/b4)*2;
	}
	
	x1 = (p/(2<<7))*(p/(2<<7));
	x1 = (x1*3038)/(2<<15);
	x2 = (-7357*p)/(2<<15);
	paPressure = p+(x1+x2+3791)/(2<<3);
}
void Bmp085::updateTemp(){
	uint8_t buf[2];
	I2C2Class::getInstance()->read(BMP085ADDR,TEMP,buf,2);
	ut = (((int32_t)buf[0])<<8)|((int32_t)buf[1]);
	
	int x1 = (ut-ac6)*(ac5)/(2<<14);
	int x2 = mc*(2<<10)/(x1+md);
	b5 = x1+x2;
	degX10Temp = (b5+8)/(2<<3);
}
float Bmp085::getPaPressure(){
	return 1.0*paPressure;
}
