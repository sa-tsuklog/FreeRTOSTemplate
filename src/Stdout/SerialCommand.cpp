#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "SerialCommand.h"
#include "PeriphLib/USART2.h"

void prvTxTask(void *pvParameters){
	portTickType xLastWakeTime = xTaskGetTickCount();

	if((USART_TypeDef*)pvParameters == USART2){
		while(1){
			USART2Class::GetInstance()->Tx();
			vTaskDelayUntil(&xLastWakeTime,1);
		}
	}
}

void prvRxTask(void *pvParameters){
	if((USART_TypeDef*)pvParameters == USART2){
		while(1){
			USART2Class::GetInstance()->Rx();
			vTaskDelay(100);

		}
	}
}

void uputc(USART_TypeDef* ch,char c){
	if(ch == USART2){
		if(xQueueSendToBackFromISR(USART2Class::GetInstance()->GetQueue(),&c,pdFALSE)!=pdPASS){
		}
	}
}

void HandleSerialCommand(char* line){
	char buf[1024];
	if(strncmp(line,"vTaskList",9)==0){
		vTaskList((signed portCHAR*)buf);
		printf("task name\tstat\tprirty\tstack\ttasknum\n\r");
		printf(buf);
	}else{
		printf("invalid command:%s\n\r",line);
	}
}
