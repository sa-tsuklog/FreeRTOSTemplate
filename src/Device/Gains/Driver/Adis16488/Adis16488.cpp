/*
 * Adis16488.cpp
 *
 *  Created on: 2014/02/19
 *      Author: sa
 */
#include <pch.h>

#include "../../ImuData.h"
#include "../../Gains.h"
#include "SPI2_TIM8.h"
#include "Adis16488.hpp"
#include "Adis16488RegisterDefs.h"
#include "../../../Util/Util.h"
#include "../../../Util/UserFlashData.h"

ADIS16488::ADIS16488(){
}

void ADIS16488::adis16488init(){
	unsigned short tmp;
	
	SPI2Class::GetInstance()->write16(PAGE_ID,0x0000);
	tmp = SPI2Class::GetInstance()->read16(PROD_ID);
	
//	//PAGE 3
	SPI2Class::GetInstance()->write16(PAGE_ID,0x0003);
	SPI2Class::GetInstance()->write16(DEC_RATE,23);			//102.5sps
	
//	//PAGE 0
	SPI2Class::GetInstance()->write16(PAGE_ID,0x0000);
}

void ADIS16488::setContinuousRead(){
	SPI2Class::GetInstance()->setTxBuf(0,SYS_E_FLAG<<8);
//	SPI2Class::GetInstance()->setTxBuf(1,X_DELTANG_LOW<<8);
//	SPI2Class::GetInstance()->setTxBuf(2,X_DELTANG_OUT<<8);
//	SPI2Class::GetInstance()->setTxBuf(3,Y_DELTANG_LOW<<8);
//	SPI2Class::GetInstance()->setTxBuf(4,Y_DELTANG_OUT<<8);
//	SPI2Class::GetInstance()->setTxBuf(5,Z_DELTANG_LOW<<8);
//	SPI2Class::GetInstance()->setTxBuf(6,Z_DELTANG_OUT<<8);
//	SPI2Class::GetInstance()->setTxBuf(7,X_DELTVEL_LOW<<8);
//	SPI2Class::GetInstance()->setTxBuf(8,X_DELTVEL_OUT<<8);
//	SPI2Class::GetInstance()->setTxBuf(9,Y_DELTVEL_LOW<<8);
//	SPI2Class::GetInstance()->setTxBuf(10,Y_DELTVEL_OUT<<8);
//	SPI2Class::GetInstance()->setTxBuf(11,Z_DELTVEL_LOW<<8);
//	SPI2Class::GetInstance()->setTxBuf(12,Z_DELTVEL_OUT<<8);
	SPI2Class::GetInstance()->setTxBuf(1,X_GYRO_LOW<<8);
	SPI2Class::GetInstance()->setTxBuf(2,X_GYRO_OUT<<8);
	SPI2Class::GetInstance()->setTxBuf(3,Y_GYRO_LOW<<8);
	SPI2Class::GetInstance()->setTxBuf(4,Y_GYRO_OUT<<8);
	SPI2Class::GetInstance()->setTxBuf(5,Z_GYRO_LOW<<8);
	SPI2Class::GetInstance()->setTxBuf(6,Z_GYRO_OUT<<8);
	SPI2Class::GetInstance()->setTxBuf(7,X_ACCL_LOW<<8);
	SPI2Class::GetInstance()->setTxBuf(8,X_ACCL_OUT<<8);
	SPI2Class::GetInstance()->setTxBuf(9,Y_ACCL_LOW<<8);
	SPI2Class::GetInstance()->setTxBuf(10,Y_ACCL_OUT<<8);
	SPI2Class::GetInstance()->setTxBuf(11,Z_ACCL_LOW<<8);
	SPI2Class::GetInstance()->setTxBuf(12,Z_ACCL_OUT<<8);
	SPI2Class::GetInstance()->setTxBuf(13,X_MAGN_OUT<<8);
	SPI2Class::GetInstance()->setTxBuf(14,Y_MAGN_OUT<<8);
	SPI2Class::GetInstance()->setTxBuf(15,Z_MAGN_OUT<<8);
	SPI2Class::GetInstance()->setTxBuf(16,BAROM_LOW<<8);
	SPI2Class::GetInstance()->setTxBuf(17,BAROM_OUT<<8);
	SPI2Class::GetInstance()->setTxBuf(18,PAGE_ID<<8);
	
	SPI2Class::GetInstance()->setContinuousMode();
}



void ADIS16488::prvAdis16488Task(void *pvParameters){
	SPI2Class::GetInstance()->init();
	ADIS16488::GetInstance()->adis16488init();
	vTaskDelay(MS_INITIAL_DELAY);
	Util* util = Util::GetInstance();
	ADIS16488::GetInstance()->setContinuousRead();
	int i=0;
	while(1){
		SPI2Class::GetInstance()->waitNewData();
	
		unsigned short* buf = SPI2Class::GetInstance()->getRxBuf();
		ImuData imuData = ImuData(
						//gyro
						+RPS_PER_LSB*((short)buf[3])+RPS_PER_LSB_LOW*buf[2],
						-RPS_PER_LSB*((short)buf[5])+RPS_PER_LSB_LOW*buf[4],
						-RPS_PER_LSB*((short)buf[7])+RPS_PER_LSB_LOW*buf[6],
						//acl
						+MPSPS_PER_LSB*((short)buf[9])+MPSPS_PER_LSB_LOW*buf[8],
						-MPSPS_PER_LSB*((short)buf[11])+MPSPS_PER_LSB_LOW*buf[10],
						-MPSPS_PER_LSB*((short)buf[13])+MPSPS_PER_LSB_LOW*buf[12],
						//magn
						+UT_PER_LSB*((short)buf[14]) - util->flashData.adisCmpsBias[0],
						-UT_PER_LSB*((short)buf[15]) - util->flashData.adisCmpsBias[1],
						-UT_PER_LSB*((short)buf[16]) - util->flashData.adisCmpsBias[2],
						//pressure
						PA_PER_LSB*((short)buf[18]) + PA_PER_LSB_LOW*buf[17],
						//isValid
						buf[1]&0x0100,
						buf[1]&0x0200,
						0.0
				);
		
		Gains::GetInstance()->appendInsData(&imuData);
		
		if(i%10==0){
			//printf("adi:%.3f,\t%.3f,\t%.3f,\t%.3f,\t%.3f,\t%.3f,\t%.3f,\t%.3f,\t%.3f\r\n",imuData.mpspsAcl.x,imuData.mpspsAcl.y,imuData.mpspsAcl.z,imuData.rpsRate.x,imuData.rpsRate.y,imuData.rpsRate.z,imuData.uTCmps.x,imuData.uTCmps.y,imuData.uTCmps.z);
			//printf("%d %f %f %f\r\n",i,imuData.rpsRate.x,imuData.rpsRate.y,imuData.rpsRate.z);
			//printf("%d %f %f %f\r\n",i,imuData.mpspsAcl.x,imuData.mpspsAcl.y,imuData.mpspsAcl.z);
			//printf("%d %d %d %d\r\n",i,(short)buf[14],(short)buf[15],(short)buf[16]);
		}
		i++;
	}
}
