/*
 * OSHook.c
 *
 *  Created on: 2014/03/14
 *      Author: Lark_mp
 */

unsigned int idle_count = 0;

#ifdef __cplusplus
 extern "C" {
#endif 

#include "Device/Util/Util.h"


void vApplicationIdleHook(void){
	idle_count++;
}

void vApplicationMallocFailedHook(void){
	while(1);
}

void vApplicationStackOverflowHook(void* ptr, char* taskname){
	printf("%s\r\n",taskname);
	vTaskDelay(100);
	while(1);
}

void vApplicationTickHook(){
	
}

void vConfigureTimerForRumTimeStats(){
	Util::GetInstance();
}

uint32_t vGetRunTimeCounterValue(){
	return Util::GetInstance()->getUsTime();
}

#ifdef __cplusplus
 }
#endif 
