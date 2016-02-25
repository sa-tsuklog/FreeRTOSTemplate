/*
 * Filter.hpp
 *
 *  Created on: 2014/03/08
 *      Author: sa
 */

#ifndef FILTER_HPP_
#define FILTER_HPP_

class Filter{
private:
	float q;
	float prewarpedCenter;
	float bxz,bxzz,byz,byzz;
	float axz,ayz;
public:
	Filter();
	Filter(float normalizedCutoff,float q);
	~Filter();
	float bandpass(float x);
	float allpass(float x);
	float getPrewarpedCenter();
	static float myAbs(float re,float im);
};

#endif /* FILTER_HPP_ */
