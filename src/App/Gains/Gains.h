/*
 * Gains.h
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */

#ifndef GAINS_H_
#define GAINS_H_

class Gains{
private:
	static int m_lattitudeRef;
	static int m_longitudeRef;
	
	
public:
	static void prvGainsTask(void *pvParameters);
	
	static float getMpspsAcl(int axis);
	static float getMpsSpeed(int axis);
	static float getMPos(int axis);
	static float getRpsRate(int axis);
	static Quaternion getAttitude();
	static int getLattitudeRef();
	static int getLongitudeRef();
};



#endif


