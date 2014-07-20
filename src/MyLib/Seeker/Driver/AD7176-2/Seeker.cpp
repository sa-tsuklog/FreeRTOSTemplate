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

Seeker::Seeker() :
m_intensity(0),
m_filter(1.591/(15.625/2),100){
	m_adDataQueue = xQueueCreate(256,sizeof(AdData));
	m_seekerDataMutex = xSemaphoreCreateMutex();

	if(m_adDataQueue == NULL || m_seekerDataMutex == NULL){

	}


	m_xLastWakeTime = xTaskGetTickCount();

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
	
	xQueueSendToBack(m_adDataQueue,&data,0);
}


portTickType Seeker::DoTask(){
	float bandpass = 0;
	float allpass = 0;
	float adData = 0;

	while(xQueueReceive(m_adDataQueue,&adData,0) != errQUEUE_EMPTY){
		bandpass = m_filter.bandpass(adData);
		allpass = m_filter.allpass(bandpass);
		if(m_decimate <128){
			Util::GetInstance()->myFprintf(0,stdout,"%d %d %d\r\n",m_decimate,(int)adData,(int)bandpass);
		}
		m_decimate=(m_decimate+1)%8196;
	}
	
	
	m_intensity = m_filter.myAbs(bandpass,allpass);
	
	if(m_decimate%200 == 0){
		//Util::GetInstance()->myFprintf(0,stdout,"%d\r\n",(int)intensity[0]);
	}
	
	m_decimate++;

	return m_xLastWakeTime;
}
