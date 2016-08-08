/*
 * MissleServoControl.h
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */

#ifndef MISSILESERVOCONTROL_H_
#define MISSILESERVOCONTROL_H_

#include <stdio.h>


class MissileServoControl {
	// Singleton pattern definition
private:
	FILE* fp;
	MissileServoControl(){
		fp = NULL;
	}
	MissileServoControl(const MissileServoControl& rhs);
	MissileServoControl& operator=(const MissileServoControl& rhs);
	virtual ~MissileServoControl() {}
public:
	static MissileServoControl* GetInstance() {
		static MissileServoControl instance;
		return &instance;
	}

	// Class definition
	enum Surface{
		TOP_RIGHT,
		BOTTOM_RIGHT,
		BOTTOM_LEFT,
		TOP_LEFT,
	};
private:
	const float SMOOTHING_FACTOR = 0.92; 
		
	int surfaceToServoCh(Surface surface);
	
public:
	void setLogOut(FILE* fp);
	void mainWingOpen();
	void mainWingLatch();
	void setPos(float pitchCommand,float rollCommand,float yawCommand);
	
	void nativeSetPos(Surface surface,float pos); //low level control function
	
};


#endif /* MISSILERSERVOCONTROL_H_ */
