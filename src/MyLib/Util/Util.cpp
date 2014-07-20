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
 * @brief flashData�̓��e���t���b�V���������ɏ�������
 */
void Util::userflashFlush(){
	FLASHClass::GetInstance()->flush((void*)&flashData,sizeof(UserflashData)/sizeof(uint32_t));
}

/**
 * @brief �}�C�N���b�P�ʂŌ��݂̎�����Ԃ��B
 * 
 * �Ԃ��l��32bit�̃t���[�����J�E���^�̒l�ł���B
 * 
 * �v���O�����̎��s���Ԃ��v������̂Ɏg�p����B
 * ��F
 * 
 * uint32_t t1 = Util::GetInstance()->getUsTime();
 * 
 * (�Ȃ�炩�̏���)
 * 
 * uint32_t t2 = Util::GetInstance()->getUsTime();
 * 
 * printf("%d[us]\r\n",t2-t1);
 * 
 * @return ���݂̎���[us]
 */
uint32_t Util::getUsTime(){
	return TIM2Class::GetInstance()->getUsTime();
}

/**
 * @brief �d�����͂̓d����Ԃ�
 * 
 * �{�֐����Ă΂��ƁA�O��v�������d����Ԃ��ƂƂ��ɁA���̌v�����J�n����B
 * 
 * ���Ȃ킿�A�{�֐��̕Ԃ�l�͑O��Ăяo�����̓d���ł���B
 * 
 * �{�֐��̌Ăяo���̕p�x���Ⴂ�ꍇ�ɂ͒��ӂ��K�v�ł���B
 * 
 * �{�֐��̏���Ăяo�����́AUtil���������������̓d����Ԃ��B
 * 
 * @return �O��Ăяo�����̓d��[V]
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
 * @brief Util���̏��������s���BUtil�ł̓^�X�N�͎g�p���Ȃ����߁A�N������Ȃ��B
 */
static void Util::initUtil(){
	Util::GetInstance();
}
