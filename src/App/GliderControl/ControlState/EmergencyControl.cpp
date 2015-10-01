/*
 * EmergencyControl.cpp
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */

#include "EmergencyControl.h"
#include "../GliderServoControl.h"
#include "MyLib/Gains/Gains.h"

EmergencyControl::EmergencyControl(){
}

void EmergencyControl::control(){
	GliderServoControl::mainWingLatch();
	GliderServoControl::setPos(1.0,0,0);
}

void EmergencyControl::reset(){
}
