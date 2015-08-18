/*
 * FlightParameters.h
 *
 *  Created on: 2015/06/06
 *      Author: sa
 */

#ifndef FLIGHTPARAMETERS_H_
#define FLIGHTPARAMETERS_H_

class FlightParameters{
public:
	float boostDGain[3];
	float degGlideAngle;
	float glidePGain[3];
	float glideDGain[3];
	float degPerMHeadingGain;
	float degPerDegHeadingToRollGain;
	float degRollCommandLimit;
};


#endif /* FLIGHTPARAMETERS_H_ */
