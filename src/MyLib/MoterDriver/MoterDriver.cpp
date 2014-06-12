/*
 * BridgeDriver.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#include "MoterDriver.h"
#include "Driver/BridgeTim3.h"
#include <stdio.h>

MoterDriver::MoterDriver(){
	BridgeTim3::GetInstance()->timerStart();
}

/**
 *  @param[power] normal mode :-1.0 to 1.0. brake mode: 0.0 to 1.0
 *  @param[direction] 0 or 1. direction of motor rotation
 *  @param[brake] 0 or 1. if 1, brake mode
 */
void MoterDriver::setPower(float torque,int direction,int brake){
	BridgeTim3* bd = BridgeTim3::GetInstance();
	int val = torque * MAX_DUTY;
	
	if(val >= MAX_DUTY){
		val = MAX_DUTY;
	}else if(torque < 0.0){
		val = 0.0;
	}
	if(brake){
		//brake mode
		bd->setDuty(0,(unsigned short)0);
		bd->setDuty(1,(unsigned short)val);
		bd->setDuty(2,(unsigned short)0);
		bd->setDuty(3,(unsigned short)65535);
		
	}else{
		if(direction){
			printf("cw %d\r\n",val);
			bd->setDuty(0,(unsigned short)0);
			bd->setDuty(1,(unsigned short)(65535));
			bd->setDuty(2,(unsigned short)(65535));
			bd->setDuty(3,(unsigned short)(val));

		}else{
			printf("ccw %d\r\n",val);
			bd->setDuty(0,(unsigned short)65535);
			bd->setDuty(1,(unsigned short)(val));
			bd->setDuty(2,(unsigned short)0);
			bd->setDuty(3,(unsigned short)(65535));
		}
	}
	
	
	
}
