/*
 * test.c
 *
 *  Created on: 2014/04/19
 *      Author: sa
 */
#include "FreeRTOS.h"
#include "task.h"
#include "test.h"
#include "stdio.h"


void prvTestTask(void* pvParameters){
	printf("%f\n\r",0.1);
	vTaskDelay(100);
}


