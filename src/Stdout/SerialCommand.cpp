/*
 * SerialCommand.cpp
 *
 *  Created on: 2014/02/16
 *      Author: sa
 */

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "HalUsart.h"
#include "SerialCommand.hpp"


char buf[1024]; 

void handleSerialCommand(char* line){
	
	
	if(strncmp(line,"vTaskList",9)==0){
		//vTaskList((signed portCHAR*)buf);
		printf("task name\tstat\tprirty\tstack\ttasknum\n\r");
		printf(buf);
	}else{
		printf("invalid command:%s\n\r",line);
	}
}


