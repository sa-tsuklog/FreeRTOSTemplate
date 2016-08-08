/*
 * MissileFlightParameters.h
 *
 *  Created on: 2016/06/17
 *      Author: sa
 */

#ifndef MISSILEFLIGHTPARAMETERS_H_
#define MISSILEFLIGHTPARAMETERS_H_

class  MissileFlightParameters{
public:
	float surfaceLimit;
			
	float holdAttitudePGain[3];
	float holdAttitudeDGain[3];
	
	float seekerPGainH;
	float seekerDGainH;
	float seekerPGainL;
	float seekerDGainL;
	float gCompensationGain;
	float manualControlDGain;
};

#endif /* MISSILEFLIGHTPARAMETERS_H_ */
