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
	uint8_t ch;
	uint32_t adData;
	
	Ad7176_2Seeker::GetInstance()->initAd7176();
	
	vTaskDelay(MS_INITIAL_DELAY);
	
	while(1){
		adData = Ad7176_2Seeker::GetInstance()->readAdData(&ch);
		
		float tmpI = bandpass[ch].bandpass((float)adData);
		float tmpQ = allpass[ch].allpass(tmpI);
		
		xSemaphoreTake(dataUpdateMutex,portMAX_DELAY);
		filteredI[ch] = tmpI;
		filteredQ[ch] = tmpQ;
		xSemaphoreGive(dataUpdateMutex);
		
	}
}

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
}

void Seeker::SeekerTaskEntry(void *pvParameters){
	Seeker::GetInstance()->SeekerTask();
}
void Seeker::initSeeker(){
	xTaskCreate(&Seeker::SeekerTaskEntry,"seeker",2048,NULL,4,NULL);
}
