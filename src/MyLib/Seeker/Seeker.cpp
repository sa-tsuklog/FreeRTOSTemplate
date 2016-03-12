/*
 * Seeker.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */
#include "FreeRTOS.h"
#include "task.h"

#include "math.h"

#include "Seeker.h"
#include "Driver/AD7176-2/Ad7176-2Seeker.h"
#include "GeneralConfig.h"

#include "MyLib/Util/Util.h"

Seeker::Seeker(){
	dataUpdateMutex = xSemaphoreCreateMutex();
	if(dataUpdateMutex == NULL){
		while(1){printf("mutex create failure\r\n");}
	}
	
	for(int i=0;i<4;i++){
		bandpass[i] = Filter(NORMALIZED_CENTER_FREQUENCY,Q_FACTOR);
		allpass[i] = Filter(NORMALIZED_CENTER_FREQUENCY,Q_FACTOR);
	}
	
	for(int i=0;i<4;i++){
		filteredI[i] = 0.0;
		filteredQ[i] = 0.0;
	}
}

void Seeker::SeekerTask(){
	static int decimation = 0;
	
	uint8_t ch;
	uint32_t adData;
	
	//seekerPritRawData();
	
	Ad7176_2Seeker::GetInstance()->initAd7176();
	
	vTaskDelay(MS_INITIAL_DELAY);
	
	static int count = 0;
	
	while(1){
		adData = Ad7176_2Seeker::GetInstance()->readAdData(&ch);
		//adData = Util::GetInstance()->getUsTime();
		
		if(ch >= 4){
			continue;
		}
		
		float tmpI = bandpass[ch].bandpass((float)adData);
		float tmpQ = allpass[ch].allpass(tmpI);
		
		
		xSemaphoreTake(dataUpdateMutex,portMAX_DELAY);
		
		filteredI[ch] = tmpI;
		filteredQ[ch] = tmpQ;
		
		xSemaphoreGive(dataUpdateMutex);
//		if(decimation == 0){
//			printf("%f,%f\r\n",filteredI[ch],filteredQ[ch]);
//		}
//		decimation = (decimation+1)%100;
	}
}

//void Seeker::seekerPritRawData(){
//	int CH_NUM = 4;
//	int BUFFER_PER_CH = 32;
//	
//	uint32_t buf[CH_NUM][BUFFER_PER_CH];
//	int32_t index[CH_NUM];
//	uint8_t ch;
//	int32_t adData;
//	
//	Ad7176_2Seeker::GetInstance()->initAd7176();
//		
//	vTaskDelay(MS_INITIAL_DELAY);
//	
//	for(int i=0;i<CH_NUM;i++){
//		for(int j=0;j<BUFFER_PER_CH;j++){
//			buf[i][j] = 0;
//		}
//	}
//	
//	while(1){
//		for(int i=0;i<CH_NUM;i++){
//			index[i] = 0;
//		}
//		
//		ch = -1;
//		while(ch != 3){
//			adData = Ad7176_2Seeker::GetInstance()->readAdData(&ch);
//		}
//		
//		for(int i=0;i<CH_NUM*BUFFER_PER_CH;i++){
//			adData = Ad7176_2Seeker::GetInstance()->readAdData(&ch);
//			buf[ch][index[ch]] = adData;
//			
//			if(index[ch] < BUFFER_PER_CH-1){
//				index[ch]++;
//			}
//		}
//		
//		for(int i=0;i<BUFFER_PER_CH;i++){
//			printf("%d,%d,%d,%d,%d\r\n",i,buf[0][i],buf[1][i],buf[2][i],buf[3][i]);
//			vTaskDelay(1);
//		}
//		printf("\r\n\r\n\r\n");
//		
//		vTaskDelay(5000);
//	}
//}

float Seeker::getIntensityOfCh(int32_t ch){
	return sqrtf(filteredI[ch]*filteredI[ch] + filteredQ[ch]*filteredQ[ch]);
}

void Seeker::getDirection(float* outUpDown,float* outLeftRight,float* outIntensity){
	volatile float tmpI[4];
	volatile float tmpQ[4];
	
	xSemaphoreTake(dataUpdateMutex,portMAX_DELAY);
	for(int32_t i=0;i<4;i++){
		tmpI[i] = filteredI[i];
		tmpQ[i] = filteredQ[i];
	}
	xSemaphoreGive(dataUpdateMutex);
	
	float intensity[4];
	for(int i=0;i<4;i++){
		intensity[i] = sqrt(tmpI[i]*tmpI[i] + tmpQ[i]*tmpQ[i]);
	}
	
	float sum = intensity[0] +intensity[1] +intensity[2] +intensity[3];
	
	*outUpDown   = (intensity[0] -intensity[1] -intensity[2] +intensity[3])/sum;
	*outLeftRight= (intensity[0] +intensity[1] -intensity[2] -intensity[3])/sum;
	*outIntensity= sum;
	
	//printf("%f,\t%f,\t%f,\t%f\r\n",intensity[0],intensity[1],intensity[2],intensity[3]);
}

void Seeker::SeekerTaskEntry(void *pvParameters){
	Seeker::GetInstance()->SeekerTask();
}
void Seeker::initSeeker(){
	xTaskCreate(&Seeker::SeekerTaskEntry,"seeker",1024,NULL,4,NULL);
}
