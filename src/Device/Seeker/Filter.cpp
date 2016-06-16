/*
 * Filter.cpp
 *
 *  Created on: 2014/03/08
 *      Author: sa
 */
#include <pch.h>
#include "Filter.hpp"

Filter::Filter(){
	prewarpedCenter=0.5106838424691942;	//for 20kHz samplerate, 1.591kHz illuminator
	q=100;
	bxz=bxzz=byz=byzz=0;
	axz=ayz=0;
}
Filter::Filter(float normalizedCutoff,float q){
	prewarpedCenter=2*tanf(normalizedCutoff*M_PI/2);
	this->q=q;
	bxz=bxzz=byz=byzz=0;
	axz=ayz=0;
}
Filter::~Filter(){
}
float Filter::bandpass(float x){
	float y;
	float wc=prewarpedCenter;

	y=( (2*wc/q)*(x-bxzz)  -  byzz*(4-2*wc/q+wc*wc) -byz*(-8+2*wc*wc)  )    /    (4+2*wc/q+wc*wc);

	bxzz=bxz;
	bxz=x;
	byzz=byz;
	byz=y;

	return y;
}

float Filter::allpass(float x){
	float y;
	float a=prewarpedCenter;

	y=((a-2)*x+(2+a)*axz+(2-a)*ayz)/(2+a);

	axz=x;
	ayz=y;

	return y;
}

float Filter::myAbs(float re,float im){
	return sqrtf(re*re+im*im);
}
float Filter::getPrewarpedCenter(){
	return prewarpedCenter;
}
