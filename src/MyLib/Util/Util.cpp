/*
 * Util.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */



#include "Util.h"
#include "Driver/ADC3.h"
#include "Driver/TIM2.h"
#include "Driver/USER_FLASH.h"
#include <stdarg.h>
#include <stdio.h>


Util::Util(){
	printfMutex = xSemaphoreCreateMutex();
	
	//USER_FLASH initialize
	initFlashData();
	
	//TIM2 initialize
	TIM2Class::GetInstance()->timerStart();
	
	//ADC2 initialize
	ADC3Class::GetInstance()->startFirstConversion();
	
	//task
	//no tasks.
}

void Util::initFlashData(){
	for(unsigned int i=0;i<sizeof(UserflashData)/sizeof(uint32_t);i++){
		*(((uint32_t*)&flashData)+i) = FLASHClass::GetInstance()->read(i);
	}
	
}

/**
 * @brief flashDataの内容をフラッシュメモリに書き込む
 */
void Util::userflashFlush(){
	FLASHClass::GetInstance()->flush((void*)&flashData,sizeof(UserflashData)/sizeof(uint32_t));
}

/**
 * @brief マイクロ秒単位で現在の時刻を返す。
 * 
 * 返す値は32bitのフリーランカウンタの値である。
 * 
 * プログラムの実行時間を計測するのに使用する。
 * 例：
 * 
 * uint32_t t1 = Util::GetInstance()->getUsTime();
 * 
 * (なんらかの処理)
 * 
 * uint32_t t2 = Util::GetInstance()->getUsTime();
 * 
 * printf("%d[us]\r\n",t2-t1);
 * 
 * @return 現在の時間[us]
 */
uint32_t Util::getUsTime(){
	return TIM2Class::GetInstance()->getUsTime();
}

/**
 * @brief 電源入力の電圧を返す
 * 
 * 本関数が呼ばれると、前回計測した電圧を返すとともに、次の計測を開始する。
 * 
 * すなわち、本関数の返り値は前回呼び出し時の電圧である。
 * 
 * 本関数の呼び出しの頻度が低い場合には注意が必要である。
 * 
 * 本関数の初回呼び出し時は、Utilを初期化した時の電圧を返す。
 * 
 * @return 前回呼び出し時の電圧[V]
 */
float Util::getVoltInputVoltage(){
	return ADC3Class::GetInstance()->getNextData();
}


void Util::myFprintf(portTickType blockTime,FILE* stream,const char* format,...){
	if(xSemaphoreTake(printfMutex,blockTime)==pdTRUE){
		va_list args;
		va_start(args,format);
		vfprintf(stream,format,args);
		va_end(args);
		xSemaphoreGive(printfMutex);
	}
}

/**
 * @brief Utilをの初期化を行う。Utilではタスクは使用しないため、起動されない。
 */
static void Util::initUtil(){
	Util::GetInstance();
}
