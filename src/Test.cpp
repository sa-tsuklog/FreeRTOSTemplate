/*
 * Test.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */
#include <pch.h>

#include "Test.h"
//dut
#include "Device/Logger/Logger.h"
#include "Device/Util/Util.h"

#include "Device/MoterDriver/MoterDriver.h"
#include "Device/Servo/Servo.h"

#include "Device/Gains/Gains.h"
#include "Device/Gains/ImuData.h"
#include "Common/Math/Quaternion.h"

#include "Device/CmdServo/CmdServo.h"

#include "Device/Propo/Driver/TIM3.h"

#include "Device/Propo/Propo.h"

#include "Device/Seeker/Driver/AD7176-2/Ad7176-2Seeker.h"
#include "Device/SignalGenerator/Driver/DAC_TIM8.h"

#include "Device/Seeker/Seeker.h"

void test(){
	int i=0;
	float updown = 0.0;
	float leftRight = 0.0;
	float intensity = 0.0;
	
	vTaskDelay(MS_INITIAL_DELAY);
	
	while(1){
		float upDownGain = 0.8;
		float leftRightGain = 0.8;
		
		float radPitch,radRoll,radHeading;
		
		Gains::GetInstance()->getAttitude().getRadPitchRollHeading(&radPitch,&radRoll,&radHeading);
		
		
		Seeker::GetInstance()->getDirectionSlow(&updown,&leftRight,&intensity);
		printf("slow:%6.3f,\t%6.3f,\t%10.1f\t,",updown*upDownGain,leftRight*leftRightGain,intensity);
		Seeker::GetInstance()->getDirectionFast(&updown,&leftRight,&intensity);
		printf("fast:%6.3f,\t%6.3f,\t%10.1f\r\n",updown*upDownGain,leftRight*leftRightGain,intensity);
		
		
		vTaskDelay(100);
		
	}
}

void prvTestTask(void* pvParamters){
	test();
}
