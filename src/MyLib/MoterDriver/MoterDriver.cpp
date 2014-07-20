/*
 * BridgeDriver.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#include "MoterDriver.h"
#include "Driver/BridgeTim3.h"
#include "Driver/BridgeTim1.h"
#include <stdio.h>

MoterDriver::MoterDriver(){
	if(USE_CH0){
		BridgeTim1::GetInstance()->timerStart();
	}
	if(USE_CH1){
		BridgeTim3::GetInstance()->timerStart();
	}
}

/**
 *  @param[power] normal mode :0.0 to 1.0. brake mode: 0.0 to 1.0
 *  @param[direction] 0 or 1. direction of motor rotation
 *  @param[brake] 0 or 1. if 1, brake mode
 */
void MoterDriver::setPower(int ch,float torque,int direction,int brake){
	BridgeTim* bd=NULL;
	
	if(USE_CH0 && ch==0){
		bd = BridgeTim1::GetInstance();
	}else if(USE_CH1 && ch == 1){
		bd = BridgeTim3::GetInstance();
	}
	
	if(bd == NULL){
		return;
	}
		
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
			bd->setDuty(0,(unsigned short)0);
			bd->setDuty(1,(unsigned short)(65535));
			bd->setDuty(2,(unsigned short)(65535));
			bd->setDuty(3,(unsigned short)(val));

		}else{
			bd->setDuty(0,(unsigned short)65535);
			bd->setDuty(1,(unsigned short)(val));
			bd->setDuty(2,(unsigned short)0);
			bd->setDuty(3,(unsigned short)(65535));
		}
	}
}
