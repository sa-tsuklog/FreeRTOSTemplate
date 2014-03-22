#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "SerialCommand.h"
#include "Drivers/PeriphLib/USART2.h"

void uputc(USART_TypeDef* ch,char c){
	if(ch == USART2){
		if(xQueueSendToBackFromISR(USART2Class::GetInstance()->GetQueue(),&c,(BaseType_t *)pdFALSE)!=pdPASS){
		}
	}
}

void HandleSerialCommand(char* line){
	char buf[1024];
	if(strncmp(line,"vTaskList",9)==0){
		vTaskList(buf);
		printf("task name\tstat\tprirty\tstack\ttasknum\n\r");
		printf(buf);
	}else{
		printf("invalid command:%s\n\r",line);
	}
}
