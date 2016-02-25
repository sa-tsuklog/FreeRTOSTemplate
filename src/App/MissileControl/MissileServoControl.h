/*
 * MissleServoControl.h
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */

#ifndef MISSILESERVOCONTROL_H_
#define MISSILESERVOCONTROL_H_

#include <stdio.h>

class MissileServoControl{
public:
	enum Surface{
			TOP_RIGHT,
			BOTTOM_RIGHT,
			BOTTOM_LEFT,
			TOP_LEFT,
		};
private:
	static const float SMOOTHING_FACTOR = 0.92; 
	
	static int surfaceToServoCh(Surface surface);
	static FILE* fp;
public:
	static void mainWingOpen();
	static void mainWingLatch();
	static void setPos(float pitchCommand,float rollCommand,float yawCommand);
	
	static void nativeSetPos(Surface surface,float pos); //low level control function
};

#endif /* MISSILERSERVOCONTROL_H_ */
