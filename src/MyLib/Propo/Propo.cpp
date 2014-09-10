/*
 * Propo.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */
#include "Driver/TIM3.h"
#include "Propo.h"

#include "stdio.h"

Propo::Propo(){
}

/**
 * @brief プロポからのパルス幅を取得する
 * 
 * 1番ピンのパルスの立ち下がりは2番ピンの立ち上がりと、2番ピンの立ち下がりは3番ピンの立ち上がりと一致していることを前提としている。
 * 
 * @param[in] ch パルス幅を読むチャンネル。0～3を指定する。
 * @return 範囲は-1.0～1.0、中立位置で0.0を返す。
 */

float Propo::getInput(int ch){
	if(USE_PROPO == 0){
		return 0.0;
	}
	
	int usDuty;
	PropoTim3* tim3 = PropoTim3::GetInstance();
	
	if(ch == 0){
		usDuty = tim3->getUsTime(0);
	}else if(ch == 1){
		usDuty = tim3->getUsTime(1);
	}else if(ch == 2){
		usDuty = tim3->getUsTime(2)-tim3->getUsTime(1);
	}else if(ch == 3){
		usDuty = tim3->getUsTime(3)-tim3->getUsTime(2);
	}else{
		usDuty = US_DUTY_CENTER;
	}
	
	
	if(usDuty < US_DUTY_CENTER-US_DUTY_GAIN){
		usDuty = US_DUTY_CENTER - US_DUTY_GAIN;
	}else if(usDuty > US_DUTY_CENTER + US_DUTY_GAIN){
		usDuty = US_DUTY_CENTER + US_DUTY_GAIN;
	}
	
	return ((float)(usDuty - US_DUTY_CENTER))/US_DUTY_GAIN;
}

/**
 * @brief プロポ信号の計測を開始する。
 */
void Propo::start(){
	if(USE_PROPO){
		PropoTim3::GetInstance()->timerStart();
	}
}

/**
 * @brief プロポ信号に必要な回路の初期化を行う。
 */
void Propo::initPropo(){
	if(USE_PROPO){
		PropoTim3::GetInstance();
		Propo::GetInstance();
	}
}
