/*
 * GliderServoControl.h
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */

#ifndef GLIDERSERVOCONTROL_H_
#define GLIDERSERVOCONTROL_H_

#include <stdio.h>

class GliderServoControl{
public:
	enum Surface{
			MAIN_WING,
			RUDDER,
			RIGHT_ELEVATOR,
			LEFT_ELEVATOR
		};
private:
	static int surfaceToServoCh(Surface surface);
	static FILE* fp;
public:
	static void mainWingOpen();
	static void mainWingLatch();
	static void setPos(float pitchCommand,float rollCommand,float yawCommand);
	
	static void nativeSetPos(Surface surface,float pos); //low level control function
};

#endif /* GLIDERSERVOCONTROL_H_ */
