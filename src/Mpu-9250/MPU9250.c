/*
 * HalI2c2.cpp
 *
 *  Created on: 2014/02/23
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
#include "MPU9250.h"
#include "Mpu9150RegisterDef.h"
#include "PeriphLib/I2C2.h"

void MPU9250InitSend(){

	I2C2Class* i2c2 = I2C2Class::GetInstance();
	i2c2->Write1(MPU9150ADDR,PWR_MGMT_1,0x80);
	i2c2->Write1(MPU9150ADDR,SIGNAL_PATH_RESET,0x07);
	vTaskDelay(10);
	i2c2->Write1(MPU9150ADDR,SIGNAL_PATH_RESET,0x00);

	i2c2->Write1(MPU9150ADDR,PWR_MGMT_1,0x00);
	i2c2->Write1(MPU9150ADDR,PWR_MGMT_2,0x00);

	i2c2->Write1(MPU9150ADDR,SMPLRT_DIV,4); //1kHz
	i2c2->Write1(MPU9150ADDR,CONFIG,0x01);
	i2c2->Write1(MPU9150ADDR,FIFO_EN,0x00);

	i2c2->Write1(MPU9150ADDR,I2C_MST_CTRL,0x00);
	i2c2->Write1(MPU9150ADDR,INT_PIN_CFG,1<<1);

	i2c2->Write1(MPU9150ADDR,GYRO_CONFIG,3<<3);
	i2c2->Write1(MPU9150ADDR,ACCEL_CONFIG,3<<3);

	i2c2->Write1(AK8975_ADDR,CNTL,0x0F);

	i2c2->Write1(AK8975_ADDR,CNTL,0x01);

}

void MPU9250Send(){
	char tmp[16];

	tmp[0]=0xC3;
	tmp[1]=0xCC;
	tmp[2]=0xFF;
	tmp[3]=0x00;
	tmp[4]=0x00;
	tmp[5]=0x01;

	I2C2Class* i2c2 = I2C2Class::GetInstance();
	i2c2->Read(MPU9150ADDR,ACCEL_XOUT_H,tmp,1);
	i2c2->Read(MPU9150ADDR,ACCEL_XOUT_L,tmp+1,1);
	i2c2->Read(MPU9150ADDR,ACCEL_YOUT_H,tmp+2,1);
	i2c2->Read(MPU9150ADDR,ACCEL_YOUT_L,tmp+3,1);
	i2c2->Read(MPU9150ADDR,ACCEL_ZOUT_H,tmp+4,1);
	i2c2->Read(MPU9150ADDR,ACCEL_ZOUT_L,tmp+5,1);

	for(int i=0;i<6;i++){
		printf("%d:read data:%x\n\r",i,tmp[i]);
	}

	printf("\n\r");
}

