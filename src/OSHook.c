/*
 * OSHook.c
 *
 *  Created on: 2014/03/14
 *      Author: Lark_mp
 */

volatile char stackoverflowTaskname[16];

void vApplicationIdleHook(void){
	static int idle_count=0;
	idle_count++;
}

void vApplicationMallocFailedHook(void){
	while(1);
}

void vApplicationStackOverflowHook(void* ptr, signed char* taskname){
	int i=0;
	while(taskname[i]!=0){
		stackoverflowTaskname[i] = taskname[i];
		i++;
	}
	while(1);
}

void vApplicationTickHook(){

}
