#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "SerialCommand.h"
#include "Drivers/PeriphLib/USART1.h"
#include "Drivers/PeriphLib/USART2.h"
#include "Drivers/PeriphLib/USART3.h"

#ifdef __cplusplus
 extern "C" {
#endif

void uputc(USART_TypeDef* ch,char c){
	if(ch == USART1){
		//if((USART1Class::GetInstance()->GetQueue()) != NULL){
			if(xQueueSendToBackFromISR(USART1Class::GetInstance()->GetQueue(),&c,(BaseType_t *)pdFALSE)!=pdPASS){
			}
		//}
	}else if(ch == USART2){
		if(USART2Class::GetInstance()->GetQueue() != NULL){
			if(xQueueSendToBackFromISR(USART2Class::GetInstance()->GetQueue(),&c,(BaseType_t *)pdFALSE)!=pdPASS){
			}
		}
	}else if(ch == USART3){
		if(USART3Class::GetInstance()->GetQueue() != NULL){
			if(xQueueSendToBackFromISR(USART3Class::GetInstance()->GetQueue(),&c,(BaseType_t *)pdFALSE)!=pdPASS){
			}
		}
	}
}

void HandleSerialCommand(char* line){
	if(strncmp(line,"vTaskList",9)==0){
		char buf[768];
		vTaskList(buf);
		printf("task name\tstat\tprirty\tstack\ttasknum\n\r");
		printf(buf);
	}else{
		printf("invalid command:%s\n\r",line);
	}
}

#ifdef __cplusplus
}
#endif
