/*
 * Servo.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */
#include "Servo.h"
#include "Driver/ServoTim1.h"
#include "Driver/ServoTim3.h"
#include "Driver/ServoTim4.h"
#include "Driver/ServoTim5.h"
#include "Device/Util/Util.h"
#include "Device/Util/UserFlashData.h"

Servo::Servo(){
	if(USE_CH0_3){
		ServoTim4::GetInstance();
	}
	if(USE_CH4_7){
		ServoTim1::GetInstance();
	}
	if(USE_CH8_11){
		ServoTim3::GetInstance();
	}
	if(USE_CH12){
		ServoTim5::GetInstance();
	}
}
/**
 * @brief パルスの出力を開始する
 */
void Servo::start(){
	if(USE_CH0_3){
		ServoTim4::GetInstance()->timerStart();
	}
	if(USE_CH4_7){
		ServoTim1::GetInstance()->timerStart();
	}
	if(USE_CH8_11){
		ServoTim3::GetInstance()->timerStart();
	}
	if(USE_CH12){
		ServoTim5::GetInstance()->timerStart();
	}
}

/**
 * @brief パルス幅を設定する
 * 
 * @param[in] ch パルス幅を設定するチャンネル
 * @param[in] pos パルス幅。-1.0～1.0。範囲外の値が入力された場合、それぞれ-1.0、1.0として扱われる。
 * 
 */
void Servo::setPos(int ch,float pos){
	int* trim = Util::GetInstance()->flashData.servoTrim;
	
	if(pos < -1.0){
		pos = -1.0;
	}else if(pos > 1.0){
		pos = 1.0;
	}
	
	switch(ch){
	case 0:if(USE_CH0_3){ServoTim4::GetInstance()->setDuty(4,US_DUTY_CENTER+pos*US_DUTY_GAIN-1 + trim[0]);}break;
	case 1:if(USE_CH0_3){ServoTim4::GetInstance()->setDuty(3,US_DUTY_CENTER+pos*US_DUTY_GAIN-1 + trim[1]);}break;
	case 2:if(USE_CH0_3){ServoTim4::GetInstance()->setDuty(2,US_DUTY_CENTER+pos*US_DUTY_GAIN-1 + trim[2]);}break;
	case 3:if(USE_CH0_3){ServoTim4::GetInstance()->setDuty(1,US_DUTY_CENTER+pos*US_DUTY_GAIN-1 + trim[3]);}break;
	case 4:if(USE_CH4_7){ServoTim1::GetInstance()->setDuty(1,US_DUTY_CENTER+pos*US_DUTY_GAIN-1 + trim[4]);}break;
	case 5:if(USE_CH4_7){ServoTim1::GetInstance()->setDuty(2,US_DUTY_CENTER+pos*US_DUTY_GAIN-1 + trim[5]);}break;
	case 6:if(USE_CH4_7){ServoTim1::GetInstance()->setDuty(3,US_DUTY_CENTER+pos*US_DUTY_GAIN-1 + trim[6]);}break;
	case 7:if(USE_CH4_7){ServoTim1::GetInstance()->setDuty(4,US_DUTY_CENTER+pos*US_DUTY_GAIN-1 + trim[7]);}break;
	case 8:if(USE_CH8_11){ServoTim3::GetInstance()->setDuty(1,US_DUTY_CENTER+pos*US_DUTY_GAIN-1 + trim[8]);}break;
	case 9:if(USE_CH8_11){ServoTim3::GetInstance()->setDuty(2,US_DUTY_CENTER+pos*US_DUTY_GAIN-1 + trim[9]);}break;
	case 10:if(USE_CH8_11){ServoTim3::GetInstance()->setDuty(3,US_DUTY_CENTER+pos*US_DUTY_GAIN-1 + trim[10]);}break;
	case 11:if(USE_CH8_11){ServoTim3::GetInstance()->setDuty(4,US_DUTY_CENTER+pos*US_DUTY_GAIN-1 + trim[11]);}break;
	case 12:if(USE_CH12){ServoTim5::GetInstance()->setDuty(2,US_DUTY_CENTER+pos*US_DUTY_GAIN-1 + trim[12]);}break;
	}
}
