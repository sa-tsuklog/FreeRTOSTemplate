/*
 * Test.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */
#include <pch.h>

#include "Test.h"
//dut
#include "MyLib/Logger/Logger.h"
#include "MyLib/Util/Util.h"

#include "MyLib/MoterDriver/MoterDriver.h"
#include "MyLib/Servo/Servo.h"

#include "MyLib/Gains/Gains.h"
#include "MyLib/Gains/ImuData.h"
#include "Common/Quaternion.h"

#include "MyLib/CmdServo/CmdServo.h"

#include "MyLib/Propo/Driver/TIM3.h"

#include "MyLib/Propo/Propo.h"

#include "MyLib/Seeker/Driver/AD7176-2/Ad7176-2Seeker.h"
#include "MyLib/SignalGenerator/Driver/DAC_TIM8.h"

#include "MyLib/Seeker/Seeker.h"

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
		
		
		Seeker::GetInstance()->getDirectionFast(&updown,&leftRight,&intensity);
		printf("%.3f,\t%.3f,\t%.3f\r\n",updown*upDownGain-radPitch,leftRight*leftRightGain-radHeading,intensity);
		
		vTaskDelay(200);
		
	}
}

#define TMP_BUFFER_SIZE 2048
unsigned int testBuf[TMP_BUFFER_SIZE];

void seekerTest(){
	printf("init\r\n");
	Ad7176_2Seeker::GetInstance()->initAd7176();
	printf("start\r\n");
	unsigned char ch;
	
	Servo::GetInstance()->start();
	float pos = 1.0;
	while(1){
		for(int i=0;i<TMP_BUFFER_SIZE;i++){
			unsigned int data = Ad7176_2Seeker::GetInstance()->readAdData(&ch);
			testBuf[i]=data;
		}
		for(int i=0;i<TMP_BUFFER_SIZE;i++){
			printf("%d\r\n",testBuf[i]);
			vTaskDelay(5);
		}
		
		vTaskDelay(1000);
		pos = pos*-1.0;
		Servo::GetInstance()->setPos(0,pos);
	}
}

void prvTestTask(void* pvParamters){
	test();
}
