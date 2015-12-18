/*
 * Seeker.cpp
 *
 *  Created on: 2014/03/08
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
	quadrantSeekerSlow = QuadrantSeeker(NORMALIZED_CENTER_FREQUENCY,Q_FACTOR_SLOW);
	quadrantSeekerFast = QuadrantSeeker(NORMALIZED_CENTER_FREQUENCY,Q_FACTOR_FAST);
}

void Seeker::SeekerTask(){
	static int decimation = 0;
	
	uint8_t ch;
	uint32_t adData;
	
	//seekerPritRawData();
	
	Ad7176_2Seeker::GetInstance()->initAd7176();
	
	vTaskDelay(MS_INITIAL_DELAY);
	
	while(1){
		adData = Ad7176_2Seeker::GetInstance()->readAdData(&ch);
		
		quadrantSeekerSlow.updateSeekerData(ch,(float)adData);
		quadrantSeekerFast.updateSeekerData(ch,(float)adData);
		
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



void Seeker::getDirectionSlow(float* outUpDown,float* outLeftRight,float* outIntensity){
	quadrantSeekerSlow.getDirection(outUpDown,outLeftRight,outIntensity);
}

void Seeker::getDirectionFast(float* outUpDown,float* outLeftRight,float* outIntensity){
	quadrantSeekerFast.getDirection(outUpDown,outLeftRight,outIntensity);
}


float Seeker::getNoiseFloorSlow(){
	return NOISE_FLOOR_SLOW;
}

float Seeker::getNoiseFloorFast(){
	return NOISE_FLOOR_FAST;
}

void Seeker::SeekerTaskEntry(void *pvParameters){
	Seeker::GetInstance()->SeekerTask();
}
void Seeker::initSeeker(){
	xTaskCreate(&Seeker::SeekerTaskEntry,"seeker",2048,NULL,4,NULL);
}
void Seeker::SeekerTaskEntry(void *pvParameters){
	Seeker::GetInstance()->SeekerTask();
}
void Seeker::initSeeker(){
	xTaskCreate(&Seeker::SeekerTaskEntry,"seeker",1024,NULL,4,NULL);
}