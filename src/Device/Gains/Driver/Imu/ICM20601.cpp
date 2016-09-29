/*
 * ICM20601.cpp
 *
 *  Created on: 2016/09/28
 *      Author: sa
 */

#include "ICM20601.h"
#include "ICM20601Reg.h"
#include "ImuSpi2.h"
#include "Device/Util/Util.h"
#include "Device/Util/UserFlashData.h"

void Icm20601::write8(uint8_t address,uint8_t data){
	uint8_t txBuf[2];
	uint8_t dummy[2];
	
	txBuf[0] = address;
	txBuf[1] = data;
	
	ImuSpi2::GetInstance()->ReadWrite(ImuSpi2::CH_ICM20601,dummy,txBuf,2);
}
uint8_t Icm20601::read8(uint8_t address){
	uint8_t txBuf[2];
	uint8_t rxBuf[2];
	
	txBuf[0] = address | 0x80;
	txBuf[1] = 0;
	
	ImuSpi2::GetInstance()->ReadWrite(ImuSpi2::CH_ICM20601,rxBuf,txBuf,2);
	
	return rxBuf[1];
}
void Icm20601::read112(uint8_t address,uint8_t* buf){
	uint8_t txBuf[15];
	uint8_t rxBuf[15];
	
	txBuf[0] = address | 0x80;
	for(int i=1;i<15;i++){
		txBuf[i] = 0;
	}
	
	ImuSpi2::GetInstance()->ReadWrite(ImuSpi2::CH_ICM20601,rxBuf,txBuf,15);
	
	for(int i=0;i<14;i++){
		buf[i] = rxBuf[i+1];
	}
}
void Icm20601::init(){
	uint8_t whoAmI = read8(Icm20601Reg::WHO_AM_I);
	if(whoAmI != 0xAF){
		printf("AK8963 communication error 0x%x\r\n",whoAmI);
		while(1);
	}
	
	write8(Icm20601Reg::PWR_MGMT_1,0x80);
	vTaskDelay(1);
	while(read8(Icm20601Reg::PWR_MGMT_1)&0x80){
		vTaskDelay(1);
	}
	
	write8(Icm20601Reg::PWR_MGMT_1,0x01);
	
	
	write8(Icm20601Reg::SMPLRT_DIV,0x00);
	write8(Icm20601Reg::CONFIG,0x04);
	write8(Icm20601Reg::GYRO_CONFIG,0x18);//2000dps
	write8(Icm20601Reg::ACCEL_CONFIG,0x18);//16g
	write8(Icm20601Reg::ACCEL_CONFIG_2,0x04);
	write8(Icm20601Reg::LP_MODE_CFG,0x09);
	
	write8(Icm20601Reg::INT_PIN_CFG,0x10);
	write8(Icm20601Reg::INT_ENABLE,0x01);
	
	write8(Icm20601Reg::USER_CTRL,0x10);
	
	write8(Icm20601Reg::PWR_MGMT_2,0x3F);
}
void Icm20601::readImu(float* outMpspsAcl,float* outRpsRate,float* outDegTemp){
	uint8_t buf[14];
	read112(Icm20601Reg::ACCEL_XOUT_H,buf);
	
	UserflashData* flash = &Util::GetInstance()->flashData;
	
	float degTemp = DEG_PER_LSB * ((short)(buf[6]<<8|buf[7])) + DEG_TEMP_OFFSET;
	
	outMpspsAcl[0]  =  MPSPS_PER_LSB * ((short)(buf[2]<<8|buf[3])) - flash->mpuAclBias[0] - degTemp * flash->mpuAclTempCoefficient[0];
	outMpspsAcl[1]  = -MPSPS_PER_LSB * ((short)(buf[0]<<8|buf[1])) - flash->mpuAclBias[1] - degTemp * flash->mpuAclTempCoefficient[1];
	outMpspsAcl[2]  =  MPSPS_PER_LSB * ((short)(buf[4]<<8|buf[5])) - flash->mpuAclBias[2] - degTemp * flash->mpuAclTempCoefficient[2];
	
	outRpsRate[0] = ( RPS_PER_LSB * ((short)(buf[10]<<8|buf[11]))) - flash->mpuGyroBias[0] - degTemp * flash->mpuGyroTempCoefficient[0];
	outRpsRate[1] = (-RPS_PER_LSB * ((short)(buf[8]<<8|buf[9]))) - flash->mpuGyroBias[1] - degTemp * flash->mpuGyroTempCoefficient[1];
	outRpsRate[2] = ( RPS_PER_LSB * ((short)(buf[12]<<8|buf[13]))) - flash->mpuGyroBias[2] - degTemp * flash->mpuGyroTempCoefficient[2];
	
	*outDegTemp = degTemp;
}


