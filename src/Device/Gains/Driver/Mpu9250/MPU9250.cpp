/*
 * HalI2c2.cpp
 *
 *  Created on: 2014/02/23
 *      Author: sa
 */
#include <pch.h>
#include "MPU9250.h"
#include "Mpu9250RegisterDefs.h"
#include "Bmp085.h"
#include "Bmp085RegisterDefs.h"
#include "I2C2.h"
#include "../../ImuData.h"
#include "../../Gains.h"
#include "Device/Util/Util.h"

Mpu9250::Mpu9250(){
	gyroCalibrationSem = xSemaphoreCreateBinary();
	aclCalibrationSem  = xSemaphoreCreateBinary();
	cmpsCalibrationSem = xSemaphoreCreateBinary();
	if(gyroCalibrationSem == NULL || aclCalibrationSem == NULL || cmpsCalibrationSem == NULL){
		while(1);
	}
	
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
	I2C2Class::getInstance()->read(AK8963_ADDR,ASAY,&(asa[1]),1);
	I2C2Class::getInstance()->read(AK8963_ADDR,ASAZ,&(asa[2]),1);
	
	for(int i=0;i<3;i++){
		cmpsGain[i] = (1.0 + (asa[i]-128.0)*0.5/128.0);
	}
	
	
	printf("%d,%d,%d\r\n",asa[0],asa[1],asa[2]);
	
	
	I2C2Class::getInstance()->write1(AK8963_ADDR,CNTL1,0x01<<4 | 0x06);
	
	if(Util::GetInstance()->flashData.imuType == ImuType::MPU9250_BMP850){
		Bmp085::GetInstance()->init();
		paPressure = Bmp085::GetInstance()->getPaPressure();
	}
	
}

void Mpu9250::startGyroCalibration(){
	xSemaphoreGive(gyroCalibrationSem);
}
void Mpu9250::startAclCalibration(){
	xSemaphoreGive(aclCalibrationSem);
}
void Mpu9250::startCmpsCalibration(){
	xSemaphoreGive(cmpsCalibrationSem);
}

void Mpu9250::readMpu9250(){
	static int count=0;
	
	unsigned char buf[14];
	unsigned char* aclBuf = buf + 0;
	unsigned char* gyroBuf = buf + 8;
	unsigned char cmpsBuf[6];
	unsigned char* tempBuf = buf + 6;

	I2C2Class* i2c2 = I2C2Class::getInstance();
	i2c2->read(MPU9250ADDR,ACCEL_XOUT_H,buf,14);
	i2c2->read(AK8963_ADDR,HXL,cmpsBuf,6);
	I2C2Class::getInstance()->write1(AK8963_ADDR,CNTL1,0x01<<4 | 0x06);
	
	UserflashData* flash = &Util::GetInstance()->flashData;
	
	degTemp = DEG_PER_LSB * ((short)(tempBuf[0]<<8|tempBuf[1])) + DEG_TEMP_OFFSET;
	
	mpspsAcl[0]  =  MPSPS_PER_LSB * ((short)(aclBuf[2]<<8|aclBuf[3])) - flash->mpuAclBias[0] - degTemp * flash->mpuAclTempCoefficient[0];
	mpspsAcl[1]  = -MPSPS_PER_LSB * ((short)(aclBuf[0]<<8|aclBuf[1])) - flash->mpuAclBias[1] - degTemp * flash->mpuAclTempCoefficient[1];
	mpspsAcl[2]  =  MPSPS_PER_LSB * ((short)(aclBuf[4]<<8|aclBuf[5])) - flash->mpuAclBias[2] - degTemp * flash->mpuAclTempCoefficient[2];
	
	rpsRate[0] = ( RPS_PER_LSB * ((short)(gyroBuf[2]<<8|gyroBuf[3]))) - flash->mpuGyroBias[0] - degTemp * flash->mpuGyroTempCoefficient[0];
	rpsRate[1] = (-RPS_PER_LSB * ((short)(gyroBuf[0]<<8|gyroBuf[1]))) - flash->mpuGyroBias[1] - degTemp * flash->mpuGyroTempCoefficient[1];
	rpsRate[2] = ( RPS_PER_LSB * ((short)(gyroBuf[4]<<8|gyroBuf[5]))) - flash->mpuGyroBias[2] - degTemp * flash->mpuGyroTempCoefficient[2];
	
	uTCmps[0] = (+UT_PER_LSB * cmpsGain[0] * ((short)(cmpsBuf[1]<<8|cmpsBuf[0]))) - flash->mpuCmpsBias[0] - degTemp * flash->mpuCmpsTempCoefficient[0];
	uTCmps[1] = (-UT_PER_LSB * cmpsGain[1] * ((short)(cmpsBuf[3]<<8|cmpsBuf[2]))) - flash->mpuCmpsBias[1] - degTemp * flash->mpuCmpsTempCoefficient[1];
	uTCmps[2] = (-UT_PER_LSB * cmpsGain[2] * ((short)(cmpsBuf[5]<<8|cmpsBuf[4]))) - flash->mpuCmpsBias[2] - degTemp * flash->mpuCmpsTempCoefficient[2];
	
	if(Util::GetInstance()->flashData.imuType == ImuType::MPU9250_BMP850){
		if(count % 2 == 0){
			Bmp085::GetInstance()->updateTemp();
			Bmp085::GetInstance()->startMeasureingPressure();
		}else{
			Bmp085::GetInstance()->updatePressure();
			Bmp085::GetInstance()->startMeasureingTemp();
			paPressure = Bmp085::GetInstance()->getPaPressure();
		}
	}
	count ++;
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
	//FILE* fp = fopen("/log2","w");
	
	int j=0;
	
	
	I2C2Class::getInstance()->start();
	
	while(1){	
		/////////////////////////////////////
		// read and enque
		/////////////////////////////////////
		I2C2Class::getInstance()->waitNewData();
		readMpu9250();
		
		ImuData imuData;
		if(Util::GetInstance()->flashData.imuType == ImuType::MPU9250_BMP850){
			imuData = ImuData(rpsRate[0],rpsRate[1],rpsRate[2],mpspsAcl[0],mpspsAcl[1],mpspsAcl[2],uTCmps[0],uTCmps[1],uTCmps[2],paPressure,1,1,degTemp);
		}else{
			imuData = ImuData(rpsRate[0],rpsRate[1],rpsRate[2],mpspsAcl[0],mpspsAcl[1],mpspsAcl[2],uTCmps[0],uTCmps[1],uTCmps[2],0,1,0,degTemp);
		}
		Gains::GetInstance()->appendInsData(&imuData);
		
		//fprintf(fp,"$INS,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f",rpsRate[0],rpsRate[1],rpsRate[2],mpspsAcl[0],mpspsAcl[1],mpspsAcl[2],uTCmps[0],uTCmps[1],uTCmps[2],0.0,1,0,degTemp);
		
		/////////////////////////////////////
		// calibration
		/////////////////////////////////////
		calibrateGyro();
		calibrateAcl();
		calibrateCmps();
		
		
		if(j==0){
			//printf("%.6f,%.6f,%.6f\r\n",rpsRate[0],rpsRate[1],rpsRate[2]);
		}
		j=(j+1)%10;
	}
}

void Mpu9250::calibrateGyro(){
	static int gyroCalibrationCount = 0;
	
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
}

void Mpu9250::calibrateAcl(){
	static int aclCalibrationCount = 0;
	
	if(xSemaphoreTake(aclCalibrationSem,0) == pdTRUE){
		printf("\r\nacl calibration start\r\n");
		
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
}

void Mpu9250::calibrateCmps(){
	static int cmpsCalibrationCount = 0;
	
	if(xSemaphoreTake(cmpsCalibrationSem,0) == pdTRUE){
		printf("Rotate the sensor to calibrate\r\n");
		cmpsCalibrationCount = SAMPLES_FOR_CALIBRATION;
		for(int i=0;i<3;i++){
			Util::GetInstance()->flashData.mpuCmpsBias[i]=0;
		}
	}
	
	if(cmpsCalibrationCount != 0){
		cmpsCalibrationCount--;
		
		for(int i=0;i<3;i++){
			cmpsCalibrationBuf[cmpsCalibrationCount][i] = uTCmps[i];
		}
		
		
		if(cmpsCalibrationCount == 0){
			float xMax,yMax,zMax,xMin,yMin,zMin;
			xMax = yMax = zMax = -1000;
			xMin = yMin = zMin = 1000;
			
			for(int i=0;i<SAMPLES_FOR_CALIBRATION;i++){
				if(cmpsCalibrationBuf[i][0] > xMax){
					xMax = cmpsCalibrationBuf[i][0];
				}
				if(cmpsCalibrationBuf[i][0] < xMin){
					xMin = cmpsCalibrationBuf[i][0];
				}
				if(cmpsCalibrationBuf[i][1] > yMax){
					yMax = cmpsCalibrationBuf[i][1];
				}
				if(cmpsCalibrationBuf[i][1] < yMin){
					yMin = cmpsCalibrationBuf[i][1];
				}
				if(cmpsCalibrationBuf[i][2] > zMax){
					zMax = cmpsCalibrationBuf[i][2];
				}
				if(cmpsCalibrationBuf[i][2] < zMin){
					zMin = cmpsCalibrationBuf[i][2];
				}
			}
			
			float bx,by,bz,mag;
			
			bx = (xMax+xMin)/2;
			by = (yMax+yMin)/2;
			bz = (zMax+zMin)/2;
			
			mag = 15.0;
			for(int i=0;i<CMPS_CALBRATION_ITERETION;i++){
				cmpsCalibrationCalcOneStep(&bx,&by,&bz,&mag);
			}
			
			printf("bx,y,z = %.3f,%.3f,%.3f\r\n",bx,by,bz);
			
			Util::GetInstance()->flashData.mpuCmpsBias[0] = bx;
			Util::GetInstance()->flashData.mpuCmpsBias[1] = by;
			Util::GetInstance()->flashData.mpuCmpsBias[2] = bz;
			Util::GetInstance()->flashData.mpuCmpsMagnitude = mag;
			
			printf("bias = (");
			for(int i=0;i<3;i++){
				printf("%.3f,",Util::GetInstance()->flashData.mpuCmpsBias[i]);
			}
			printf("), ");
			printf("magnitude = %.3f\r\n",Util::GetInstance()->flashData.mpuCmpsMagnitude);
			
			Util::GetInstance()->userflashFlush();
		}
	}
	
}

void Mpu9250::cmpsCalibrationCalcOneStep(float* ioBxBuf,float* ioByBuf,float* ioBzBuf,float* ioMagnitudeBuf){
	float rTmp = *ioMagnitudeBuf;
	float bx = *ioBxBuf;
	float by = *ioByBuf;
	float bz = *ioBzBuf;
	
	float rAve = 0;
	for(int i=0;i<SAMPLES_FOR_CALIBRATION;i++){
		rAve += sqrtf(powf(cmpsCalibrationBuf[i][0]-bx,2)+powf(cmpsCalibrationBuf[i][1]-by,2)+powf(cmpsCalibrationBuf[i][2]-bz,2));
	}
	rAve /= SAMPLES_FOR_CALIBRATION;
	
	float xAve = 0;
	for(int i=0;i<SAMPLES_FOR_CALIBRATION;i++){
		xAve += cmpsCalibrationBuf[i][0];
	}
	xAve /= SAMPLES_FOR_CALIBRATION;
	
	float yAve = 0;
	for(int i=0;i<SAMPLES_FOR_CALIBRATION;i++){
		yAve += cmpsCalibrationBuf[i][1];
	}
	yAve /= SAMPLES_FOR_CALIBRATION;
	
	float zAve = 0;
	for(int i=0;i<SAMPLES_FOR_CALIBRATION;i++){
		zAve += cmpsCalibrationBuf[i][2];
	}
	zAve /= SAMPLES_FOR_CALIBRATION;
	
	float laAve = 0;
	for(int i=0;i<SAMPLES_FOR_CALIBRATION;i++){
		laAve -= (cmpsCalibrationBuf[i][0]-bx)/sqrtf(powf(cmpsCalibrationBuf[i][0]-bx,2)+powf(cmpsCalibrationBuf[i][1]-by,2)+powf(cmpsCalibrationBuf[i][2]-bz,2));
	}
	laAve /= SAMPLES_FOR_CALIBRATION;
	
	float lbAve = 0;
	for(int i=0;i<SAMPLES_FOR_CALIBRATION;i++){
		lbAve -= (cmpsCalibrationBuf[i][1]-by)/sqrtf(powf(cmpsCalibrationBuf[i][0]-bx,2)+powf(cmpsCalibrationBuf[i][1]-by,2)+powf(cmpsCalibrationBuf[i][2]-bz,2));
	}
	lbAve /= SAMPLES_FOR_CALIBRATION;
	
	float lcAve = 0;
	for(int i=0;i<SAMPLES_FOR_CALIBRATION;i++){
		lcAve -= (cmpsCalibrationBuf[i][2]-bz)/sqrtf(powf(cmpsCalibrationBuf[i][0]-bx,2)+powf(cmpsCalibrationBuf[i][1]-by,2)+powf(cmpsCalibrationBuf[i][2]-bz,2));
	}
	lcAve /= SAMPLES_FOR_CALIBRATION;
	
	*ioMagnitudeBuf =  rAve;
	*ioBxBuf = xAve + rAve*laAve;
	*ioByBuf = yAve + rAve*lbAve;
	*ioBzBuf = zAve + rAve*lcAve;
}


void prvMpu9250TaskEntry(void *pvParameters){
	Mpu9250::getInstance()->prvMpu9250Task(pvParameters);
}

