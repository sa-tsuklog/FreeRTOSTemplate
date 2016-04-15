/*
 * QuadrantSeeker.cpp
 *
 *  Created on: 2016/04/15
 *      Author: sa
 */

#include "math.h"

#include "QuadrantSeeker.h"



QuadrantSeeker::QuadrantSeeker(){
	for(int32_t i =0 ;i<4;i++){
		bandpass[i] = Filter();
		allpass[i]  = Filter();
		intensity[i] = 0;
	}
}
QuadrantSeeker::QuadrantSeeker(float normalizedCenterFreq,float qFactor){
	for(int32_t i =0 ;i<4;i++){
		bandpass[i] = Filter(normalizedCenterFreq,qFactor);
		allpass[i]  = Filter(normalizedCenterFreq,qFactor);
		intensity[i] = 0;
	}
}

float QuadrantSeeker::getIntensityOfCh(int32_t ch){
	if(ch < 4 && ch >= 0){
		return intensity[ch];
	}else{
		return 0;
	}
}

void QuadrantSeeker::updateSeekerData(int32_t ch,float adData){
	if(ch >= 4){
		return;
	}
	
	float tmpI = bandpass[ch].bandpass(adData);
	float tmpQ = allpass[ch].allpass(tmpI);
	
	intensity[ch] = sqrtf(tmpI*tmpI + tmpQ*tmpQ);
}


void QuadrantSeeker::getDirection(float* outUpDown,float* outLeftRight,float* outIntensity){
	float sum = intensity[0] +intensity[1] +intensity[2] +intensity[3];
	
	*outUpDown   = (intensity[0] -intensity[1] -intensity[2] +intensity[3])/sum;
	*outLeftRight= (intensity[0] +intensity[1] -intensity[2] -intensity[3])/sum;
	*outIntensity= sum;
	
	//printf("%f,\t%f,\t%f,\t%f\r\n",intensity[0],intensity[1],intensity[2],intensity[3]);
}
