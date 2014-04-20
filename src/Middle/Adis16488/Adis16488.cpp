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

ADIS16488::ADIS16488(){
}

void ADIS16488::adis16488init(){
	unsigned short tmp;
	
	tmp = SPI1Class::GetInstance()->read16(PROD_ID);
	printf("%d\n\r",tmp);
	
	//PAGE 3
	
	
	
	SPI1Class::GetInstance()->write16(PAGE_ID,0x0003);
	SPI1Class::GetInstance()->write16(DEC_RATE,23);			//102.5sps
	
	//PAGE 0
	SPI1Class::GetInstance()->write16(PAGE_ID,0x0000);
}

void ADIS16488::setContinuousRead(){
//	SPI1Class::GetInstance()->setTxBuf(0,X_DELTANG_LOW<<8);
//	SPI1Class::GetInstance()->setTxBuf(1,X_DELTANG_OUT<<8);
//	SPI1Class::GetInstance()->setTxBuf(2,Y_DELTANG_LOW<<8);
//	SPI1Class::GetInstance()->setTxBuf(3,Y_DELTANG_OUT<<8);
//	SPI1Class::GetInstance()->setTxBuf(4,Z_DELTANG_LOW<<8);
//	SPI1Class::GetInstance()->setTxBuf(5,Z_DELTANG_OUT<<8);
//	SPI1Class::GetInstance()->setTxBuf(6,X_DELTVEL_LOW<<8);
//	SPI1Class::GetInstance()->setTxBuf(7,X_DELTVEL_OUT<<8);
//	SPI1Class::GetInstance()->setTxBuf(8,Y_DELTVEL_LOW<<8);
//	SPI1Class::GetInstance()->setTxBuf(9,Y_DELTVEL_OUT<<8);
//	SPI1Class::GetInstance()->setTxBuf(10,Z_DELTVEL_LOW<<8);
//	SPI1Class::GetInstance()->setTxBuf(11,Z_DELTVEL_OUT<<8);
	SPI1Class::GetInstance()->setTxBuf(0,X_GYRO_LOW<<8);
	SPI1Class::GetInstance()->setTxBuf(1,X_GYRO_OUT<<8);
	SPI1Class::GetInstance()->setTxBuf(2,Y_GYRO_LOW<<8);
	SPI1Class::GetInstance()->setTxBuf(3,Y_GYRO_OUT<<8);
	SPI1Class::GetInstance()->setTxBuf(4,Z_GYRO_LOW<<8);
	SPI1Class::GetInstance()->setTxBuf(5,Z_GYRO_OUT<<8);
	SPI1Class::GetInstance()->setTxBuf(6,X_ACCL_LOW<<8);
	SPI1Class::GetInstance()->setTxBuf(7,X_ACCL_OUT<<8);
	SPI1Class::GetInstance()->setTxBuf(8,Y_ACCL_LOW<<8);
	SPI1Class::GetInstance()->setTxBuf(9,Y_ACCL_OUT<<8);
	SPI1Class::GetInstance()->setTxBuf(10,Z_ACCL_LOW<<8);
	SPI1Class::GetInstance()->setTxBuf(11,Z_ACCL_OUT<<8);
	SPI1Class::GetInstance()->setTxBuf(12,X_MAGN_OUT<<8);
	SPI1Class::GetInstance()->setTxBuf(13,Y_MAGN_OUT<<8);
	SPI1Class::GetInstance()->setTxBuf(14,Z_MAGN_OUT<<8);
	SPI1Class::GetInstance()->setTxBuf(15,BAROM_LOW<<8);
	SPI1Class::GetInstance()->setTxBuf(16,BAROM_OUT<<8);
	SPI1Class::GetInstance()->setTxBuf(17,SYS_E_FLAG<<8);
	SPI1Class::GetInstance()->setTxBuf(18,PAGE_ID<<8);
	
	SPI1Class::GetInstance()->setContinuousMode();
}



void ADIS16488::prvAdis16488Task(void *pvParameters){
	ADIS16488::GetInstance()->adis16488init();
	ADIS16488::GetInstance()->setContinuousRead();
	int i=0;
	while(1){
		//SPI1Class::GetInstance()->timerStart();
		SPI1Class::GetInstance()->waitNewData();
		
		unsigned short* buf = SPI1Class::GetInstance()->getRxBuf();
		ImuData imuData = ImuData(
						//gyro
						RPS_PER_LSB*((short)buf[2])+RPS_PER_LSB_LOW*buf[3]-RPS_GYROBIAS_X,
						RPS_PER_LSB*((short)buf[4])+RPS_PER_LSB_LOW*buf[5]-RPS_GYROBIAS_Y,
						RPS_PER_LSB*((short)buf[6])+RPS_PER_LSB_LOW*buf[7]-RPS_GYROBIAS_Z,
						//acl
						MPSPS_PER_LSB*((short)buf[8])+MPSPS_PER_LSB_LOW*buf[9],
						MPSPS_PER_LSB*((short)buf[10])+MPSPS_PER_LSB_LOW*buf[11],
						MPSPS_PER_LSB*((short)buf[12])+MPSPS_PER_LSB_LOW*buf[13],
						//magn
						UT_PER_LSB*buf[13],
						UT_PER_LSB*buf[14],
						UT_PER_LSB*buf[15],
						//pressure
						PA_PER_LSB*(((int)buf[17])<<16 | (((int)buf[16])&0x00FF)),
						//isValid
						buf[18]&0x0000,
						buf[18]&0x0000
				);
		if(i%10==0){
			//printf("%d %f %f %f\n\r",i,imuData.rpsRate[0],imuData.rpsRate[1],imuData.rpsRate[2]);
			printf("%d %f %f %f\n\r",i,imuData.mpspsAcl[0],imuData.mpspsAcl[1],imuData.mpspsAcl[2]);
		}
		i++;
	}
}
