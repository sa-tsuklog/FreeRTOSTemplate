/*
 * QuadrantSeeker.h
 *
 *  Created on: 2016/04/15
 *      Author: sa
 */

#ifndef QUADRANTSEEKER_H_
#define QUADRANTSEEKER_H_

#include "stdint.h"

#include "Filter.hpp"

class QuadrantSeeker{
private:
	Filter bandpass[4];
	Filter allpass[4];
	float intensity[4];
	
	float getIntensityOfCh(int32_t ch);
public:
	QuadrantSeeker();
	QuadrantSeeker(float normalizedCenterFreq,float qFactor);
	
	void updateSeekerData(int32_t ch,float adData);
	void getDirection(float* outUpDown,float* outLeftRight,float* outIntensity);
};


#endif /* QUADRANTSEEKER_H_ */
