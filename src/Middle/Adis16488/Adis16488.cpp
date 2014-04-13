/*
 * Adis16488.cpp
 *
 *  Created on: 2014/02/19
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
#include "../../App/Gains/ImuData.h"
#include "Drivers/PeriphLib/SPI1_TIM1.h"
#include "Middle/Adis16488/Adis16488.hpp"
#include "Middle/Adis16488/Adis16488RegisterDefs.h"

void write16(unsigned char address,unsigned short writedata){
	
}
unsigned short read16(unsigned char address){
	return 0;
}

void setBuf(int index,char adr){
	
}

void adis16488init(){
	unsigned short tmp;
	tmp = read16(PROD_ID);
	printf("%d\n\r",tmp);
	
	//PAGE 3
	write16(PAGE_ID,0x0003);	
	write16(DEC_RATE,23);	//102.5sps
	
	//PAGE 0
	write16(PAGE_ID,0x0000);
}

void Adis16488Read(){
	setBuf(0,X_DELTANG_LOW);
	setBuf(1,X_DELTANG_OUT);
	setBuf(2,Y_DELTANG_LOW);
	setBuf(3,Y_DELTANG_OUT);
	setBuf(4,Z_DELTANG_LOW);
	setBuf(5,Z_DELTANG_OUT);
	setBuf(6,X_DELTVEL_LOW);
	setBuf(7,X_DELTVEL_OUT);
	setBuf(8,Y_DELTVEL_LOW);
	setBuf(9,Y_DELTVEL_OUT);
	setBuf(10,Z_DELTVEL_LOW);
	setBuf(11,Z_DELTVEL_OUT);
	setBuf(12,X_MAGN_OUT);
	setBuf(13,Y_MAGN_OUT);
	setBuf(14,Z_MAGN_OUT);
	setBuf(15,BAROM_LOW);
	setBuf(16,BAROM_OUT);
	setBuf(17,SYS_E_FLAG);
	setBuf(18,PAGE_ID);
	
}



void ADIS16488::prvAdis16488Task(void *pvParameters){
	adis16488init();
	while(1){
		SPI1Class::GetInstance()->waitNewData();
//		unsigned short* buf = SPI1Class::GetInstance()->getRxBuf();
//		ImuData imuData = ImuData(
//						//gyro
//						(((int)buf[2])<<16 | (((int)buf[1])&0x00FF)),
//						(((int)buf[4])<<16 | (((int)buf[3])&0x00FF)),
//						(((int)buf[6])<<16 | (((int)buf[5])&0x00FF)),
//						//acl
//						(((int)buf[8])<<16 | (((int)buf[7])&0x00FF)),
//						(((int)buf[10])<<16 | (((int)buf[9])&0x00FF)),
//						(((int)buf[12])<<16 | (((int)buf[11])&0x00FF)),
//						//magn
//						buf[13],
//						buf[14],
//						buf[15],
//						//pressure
//						(((int)buf[17])<<16 | (((int)buf[16])&0x00FF)),
//						//isValid
//						buf[18]&0x0000,
//						buf[18]&0x0000
//				);
		
	}
}
