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
#include "Mpu9250RegisterDefs.h"
#include "I2C2.h"

Mpu9250::Mpu9250(){
}

void Mpu9250::init(){
	I2C2Class::getInstance()->write1(MPU9250ADDR,PWR_MGMT_1,0x80);
	I2C2Class::getInstance()->write1(MPU9250ADDR,SIGNAL_PATH_RESET,0x07);
	vTaskDelay(10);
	I2C2Class::getInstance()->write1(MPU9250ADDR,SIGNAL_PATH_RESET,0x00);
	I2C2Class::getInstance()->write1(MPU9250ADDR,PWR_MGMT_1,0x00);
	I2C2Class::getInstance()->write1(MPU9250ADDR,PWR_MGMT_2,0x00);
	I2C2Class::getInstance()->write1(MPU9250ADDR,SMPLRT_DIV,4); //1kHz
	I2C2Class::getInstance()->write1(MPU9250ADDR,CONFIG,0x04);	//20Hz bandwidth
	I2C2Class::getInstance()->write1(MPU9250ADDR,GYRO_CONFIG,1<<3|3);//500[deg/s]FS,USE LPF
	I2C2Class::getInstance()->write1(MPU9250ADDR,ACCEL_CONFIG,3<<3);//16[g]FS
	I2C2Class::getInstance()->write1(MPU9250ADDR,ACCEL_CONFIG2,0<<3|4);//20Hz bandwidth
	I2C2Class::getInstance()->write1(MPU9250ADDR,FIFO_EN,0x00);
	I2C2Class::getInstance()->write1(MPU9250ADDR,I2C_MST_CTRL,0x00);
	I2C2Class::getInstance()->write1(MPU9250ADDR,USER_CTRL,0x00);
	I2C2Class::getInstance()->write1(MPU9250ADDR,INT_PIN_CFG,2);//bypass
	I2C2Class::getInstance()->write1(MPU9250ADDR,INT_ENABLE,0);
	
	vTaskDelay(1);
	I2C2Class::getInstance()->write1(AK8963_ADDR,CNTL2,0x01);
	vTaskDelay(1);
	I2C2Class::getInstance()->write1(AK8963_ADDR,CNTL1,0x0F);
	
	

	unsigned char asa[3];
	I2C2Class::getInstance()->read(AK8963_ADDR,ASAX,&(asa[0]),1);
	I2C2Class::getInstance()->read(AK8963_ADDR,ASAX,&(asa[1]),1);
	I2C2Class::getInstance()->read(AK8963_ADDR,ASAX,&(asa[2]),1);
	
	for(int i=0;i<3;i++){
		cmpsGain[i] = (1.0 + (asa[i]-128.0)*0.5/128.0);
	}
	
	
	I2C2Class::getInstance()->write1(AK8963_ADDR,CNTL1,0x01<<4 | 0x06);
	
}

void Mpu9250::readMpu9250(){
	unsigned char aclBuf[6];
	unsigned char gyroBuf[6];
	unsigned char cmpsBuf[6];
	unsigned char tempBuf[2];

	I2C2Class* i2c2 = I2C2Class::getInstance();
	i2c2->read(MPU9250ADDR,ACCEL_XOUT_H,aclBuf,1);
	i2c2->read(MPU9250ADDR,ACCEL_XOUT_L,aclBuf+1,1);
	i2c2->read(MPU9250ADDR,ACCEL_YOUT_H,aclBuf+2,1);
	i2c2->read(MPU9250ADDR,ACCEL_YOUT_L,aclBuf+3,1);
	i2c2->read(MPU9250ADDR,ACCEL_ZOUT_H,aclBuf+4,1);
	i2c2->read(MPU9250ADDR,ACCEL_ZOUT_L,aclBuf+5,1);
	i2c2->read(MPU9250ADDR,GYRO_XOUT_H,gyroBuf,1);
	i2c2->read(MPU9250ADDR,GYRO_XOUT_L,gyroBuf+1,1);
	i2c2->read(MPU9250ADDR,GYRO_YOUT_H,gyroBuf+2,1);
	i2c2->read(MPU9250ADDR,GYRO_YOUT_L,gyroBuf+3,1);
	i2c2->read(MPU9250ADDR,GYRO_ZOUT_H,gyroBuf+4,1);
	i2c2->read(MPU9250ADDR,GYRO_ZOUT_L,gyroBuf+5,1);
	i2c2->read(MPU9250ADDR,TEMP_OUT_H,tempBuf,1);
	i2c2->read(MPU9250ADDR,TEMP_OUT_L,tempBuf+1,1);
	i2c2->read(AK8963_ADDR,HXH,cmpsBuf,1);
	i2c2->read(AK8963_ADDR,HXL,cmpsBuf+1,1);
	i2c2->read(AK8963_ADDR,HYH,cmpsBuf+2,1);
	i2c2->read(AK8963_ADDR,HYL,cmpsBuf+3,1);
	i2c2->read(AK8963_ADDR,HZH,cmpsBuf+4,1);
	i2c2->read(AK8963_ADDR,HZL,cmpsBuf+5,1);
	I2C2Class::getInstance()->write1(AK8963_ADDR,CNTL1,0x01<<4 | 0x06);
	
	
	float acl[3];
	float cmps[3];
	float gyro[3];
	float temp;
	for(int i=0;i<3;i++){
		acl[i] = (short)(aclBuf[i*2]<<8|aclBuf[i*2+1]);
		cmps[i] = (short)(cmpsBuf[i*2]<<8|cmpsBuf[i*2+1]);
		gyro[i] = (short)(gyroBuf[i*2]<<8|gyroBuf[i*2+1]);
	}
	temp = (short)(tempBuf[0]<<8|tempBuf[1]);
	
	//printf("%f,%f,%f\r\n",acl[0],acl[1],acl[2]);
	//printf("%f,%f,%f\r\n",cmps[0],cmps[1],cmps[2]);

	//printf("\r\n");
}

void Mpu9250::prvMpu9250Task(void* pvParameters){
	I2C2Class::getInstance();
	init();
	vTaskDelay(100);
	while(1){
		unsigned char tmp=0xA5;
		unsigned char tmp2=0xA5;
		I2C2Class::getInstance()->read(MPU9250ADDR,WHO_AM_I,&tmp,1);
		I2C2Class::getInstance()->read(AK8963_ADDR,WIA,&tmp2,1);
		
		//printf("%x,%x\r\n",tmp,tmp2);
		
		readMpu9250();
		vTaskDelay(100);
	}
}

void prvMpu9250TaskEntry(void *pvParameters){
	Mpu9250::getInstance()->prvMpu9250Task(pvParameters);
}

