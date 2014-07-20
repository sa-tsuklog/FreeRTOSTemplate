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
#include "../../ImuData.h"
#include "../../Gains.h"
#include "MyLib/Util/Util.h"

Mpu9250::Mpu9250(){
	gyroCalibrationSem = xSemaphoreCreateBinary();
	aclCalibrationSem  = xSemaphoreCreateBinary();
}

void Mpu9250::init(){
	I2C2Class::getInstance()->write1(MPU9250ADDR,PWR_MGMT_1,0x80);
	I2C2Class::getInstance()->write1(MPU9250ADDR,SIGNAL_PATH_RESET,0x07);
	vTaskDelay(10);
	I2C2Class::getInstance()->write1(MPU9250ADDR,SIGNAL_PATH_RESET,0x00);
	I2C2Class::getInstance()->write1(MPU9250ADDR,PWR_MGMT_1,0x00);
	I2C2Class::getInstance()->write1(MPU9250ADDR,PWR_MGMT_2,0x00);
	I2C2Class::getInstance()->write1(MPU9250ADDR,SMPLRT_DIV,9); //1kHz/10
	I2C2Class::getInstance()->write1(MPU9250ADDR,CONFIG,0x04);	//20Hz bandwidth
	I2C2Class::getInstance()->write1(MPU9250ADDR,GYRO_CONFIG,1<<3|0);//500[deg/s]FS,USE LPF
	I2C2Class::getInstance()->write1(MPU9250ADDR,ACCEL_CONFIG,3<<3);//16[g]FS
	I2C2Class::getInstance()->write1(MPU9250ADDR,ACCEL_CONFIG2,0<<3|4);//20Hz bandwidth
	I2C2Class::getInstance()->write1(MPU9250ADDR,FIFO_EN,0x00);
	I2C2Class::getInstance()->write1(MPU9250ADDR,I2C_MST_CTRL,0x00);
	I2C2Class::getInstance()->write1(MPU9250ADDR,USER_CTRL,0x00);
	I2C2Class::getInstance()->write1(MPU9250ADDR,INT_PIN_CFG,0x12);//bypass
	I2C2Class::getInstance()->write1(MPU9250ADDR,INT_ENABLE,1);
	
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

void Mpu9250::startGyroCalibration(){
	xSemaphoreGive(gyroCalibrationSem);
}
void Mpu9250::startAclCalibration(){
	xSemaphoreGive(aclCalibrationSem);
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
	
	degTemp = DEG_PER_LSB * ((short)(tempBuf[0]<<8|tempBuf[1]));
	
	mpspsAcl[0]  =  MPSPS_PER_LSB * ((short)(aclBuf[2]<<8|aclBuf[3])) - Util::GetInstance()->flashData.mpuAclBias[0];
	mpspsAcl[1]  = -MPSPS_PER_LSB * ((short)(aclBuf[0]<<8|aclBuf[1])) - Util::GetInstance()->flashData.mpuAclBias[1];
	mpspsAcl[2]  =  MPSPS_PER_LSB * ((short)(aclBuf[4]<<8|aclBuf[5])) - Util::GetInstance()->flashData.mpuAclBias[2];
	
	rpsRate[0] = ( RPS_PER_LSB * ((short)(gyroBuf[2]<<8|gyroBuf[3]))) - Util::GetInstance()->flashData.mpuGyroBias[0];
	rpsRate[1] = (-RPS_PER_LSB * ((short)(gyroBuf[0]<<8|gyroBuf[1]))) - Util::GetInstance()->flashData.mpuGyroBias[1];
	rpsRate[2] = ( RPS_PER_LSB * ((short)(gyroBuf[4]<<8|gyroBuf[5]))) - Util::GetInstance()->flashData.mpuGyroBias[2];
	
	uTCmps[0] = (+UT_PER_LSB * ((short)(cmpsBuf[0]<<8|cmpsBuf[1]))) - Util::GetInstance()->flashData.mpuCmpsBias[0];
	uTCmps[1] = (-UT_PER_LSB * ((short)(cmpsBuf[2]<<8|cmpsBuf[3]))) - Util::GetInstance()->flashData.mpuCmpsBias[1];
	uTCmps[2] = (-UT_PER_LSB * ((short)(cmpsBuf[4]<<8|cmpsBuf[5]))) - Util::GetInstance()->flashData.mpuCmpsBias[2];
}

void Mpu9250::prvMpu9250Task(void* pvParameters){
	I2C2Class::getInstance();
	init();
	vTaskDelay(100);
	unsigned char tmp=0xA5;
	unsigned char tmp2=0xA5;
	I2C2Class::getInstance()->read(MPU9250ADDR,WHO_AM_I,&tmp,1);
	I2C2Class::getInstance()->read(AK8963_ADDR,WIA,&tmp2,1);
	
	Util::GetInstance()->myFprintf(0,stdout,"%x,%x\r\n",tmp,tmp2);
	
	int j=0;
	int gyroCalibrationCount = 0;
	int aclCalibrationCount = 0;
	I2C2Class::getInstance()->start();
	
	while(1){	
		/////////////////////////////////////
		// read and enque
		/////////////////////////////////////
		I2C2Class::getInstance()->waitNewData();
		readMpu9250();
		ImuData imuData = ImuData(rpsRate[0],rpsRate[1],rpsRate[2],mpspsAcl[0],mpspsAcl[1],mpspsAcl[2],uTCmps[0],uTCmps[1],uTCmps[2],0.0,1,0);
		Gains::GetInstance()->appendInsData(&imuData);
		
		/////////////////////////////////////
		// calibration
		/////////////////////////////////////
		if(xSemaphoreTake(gyroCalibrationSem,0) == pdTRUE){
			printf("\r\ncalibration start\r\n");
			
			gyroCalibrationCount = SAMPLES_FOR_CALIBRATION;
			for(int i=0;i<3;i++){
				calibrationBuf[i]=0;
				Util::GetInstance()->flashData.mpuGyroBias[i]=0;
			}
		}
		if(gyroCalibrationCount != 0){
			for(int i=0;i<3;i++){
				calibrationBuf[i] += rpsRate[i];
			}
			gyroCalibrationCount--;
			
			if(gyroCalibrationCount==0){
				for(int i=0;i<3;i++){
					Util::GetInstance()->flashData.mpuGyroBias[i]=calibrationBuf[i]/SAMPLES_FOR_CALIBRATION;
				}
				Util::GetInstance()->userflashFlush();
				
				printf("\r\ncalibration end %.3f,%.3f,%.3f\r\n\r\n",Util::GetInstance()->flashData.mpuGyroBias[0],Util::GetInstance()->flashData.mpuGyroBias[1],Util::GetInstance()->flashData.mpuGyroBias[2]);
				
			}
		}
		if(xSemaphoreTake(aclCalibrationSem,0) == pdTRUE){
			printf("\r\ncalibration start\r\n");
			
			aclCalibrationCount = SAMPLES_FOR_CALIBRATION;
			for(int i=0;i<3;i++){
				calibrationBuf[i]=0;
				Util::GetInstance()->flashData.mpuAclBias[i]=0;
			}
		}
		if(aclCalibrationCount != 0){
			for(int i=0;i<3;i++){
				calibrationBuf[i] += mpspsAcl[i];
			}
			aclCalibrationCount--;
			
			if(aclCalibrationCount==0){
				printf("\r\n");
				for(int i=0;i<3;i++){
					if(calibrationBuf[i]/SAMPLES_FOR_CALIBRATION <1.0 && -1.0 < calibrationBuf[i]/SAMPLES_FOR_CALIBRATION){
						Util::GetInstance()->flashData.mpuAclBias[i] = calibrationBuf[i]/SAMPLES_FOR_CALIBRATION;
						
						printf("axis%d:%.3f\r\n",i,Util::GetInstance()->flashData.mpuAclBias[i]);
					}
				}
				printf("\r\n");
				Util::GetInstance()->userflashFlush();
			}
		}
		
		
		if(j==0){
			//printf("%.6f,%.6f,%.6f\r\n",rpsRate[0],rpsRate[1],rpsRate[2]);
		}
		j=(j+1)%10;
	}
}

void prvMpu9250TaskEntry(void *pvParameters){
	Mpu9250::getInstance()->prvMpu9250Task(pvParameters);
}

