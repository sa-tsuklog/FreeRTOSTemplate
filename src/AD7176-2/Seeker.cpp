/*
 * Seeker.cpp
 *
 *  Created on: 2014/03/08
 *      Author: sa
 */

#include "stdio.h"
#include "stdlib.h"
#include "Seeker.hpp"
#include "AdData.hpp"

Seeker::Seeker(){
	m_adDataQueue = xQueueCreate(256,sizeof(AdData));
	m_seekerDataMutex = xSemaphoreCreateMutex();

	if(m_adDataQueue == NULL || m_seekerDataMutex == NULL){

	}


	for(int i=0;i<4;i++){
		m_filter[i] = new Filter(1.591/(10/4.0),100);

		if(m_filter[i] == NULL){
			printf("malloc error at prvSeekerTask\n\r");
			while(1){}
		}

	}

	m_xLastWakeTime = xTaskGetTickCount();

	int j;
	float* buf1 = (float*)malloc(sizeof(float)*128);
	float* buf2 = (float*)malloc(sizeof(float)*128);
	for(int i=0;i<128;i++){
		buf1[i] = 0;
		buf2[i] = 0;
	}

	m_decimate = 0;
}

void Seeker::EnqueAdData(float* data){
	if(m_adDataQueue == NULL){
		return;
	}
	
	AdData adData;
	for(int i=0;i<4;i++){
		adData.ch[i] = data[i];
	}
	xQueueSendToBack(m_adDataQueue,&adData,0);
}

portTickType Seeker::DoTask(){
	float bandpass[4];
	float allpass[4];
	AdData adData;

	while(xQueueReceive(m_adDataQueue,&adData,0) != errQUEUE_EMPTY){
		for(int i=0;i<4;i++){
			bandpass[i] = m_filter[i]->bandpass(adData.ch[i]);
			allpass[i] = m_filter[i]->allpass(bandpass[i]);
		}
	}

	for(int i=0;i<4;i++){
		m_intensity[i] = m_filter[i]->myAbs(bandpass[i],allpass[i]);
	}
	if(m_decimate%100 == 0){
		printf("%f\n\r",m_intensity[0]);
	}

	m_decimate++;

	return m_xLastWakeTime;
}
