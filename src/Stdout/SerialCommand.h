#ifndef __HAL_USART_H__
#define __HAL_USART_H__

#include "stm32f4xx.h"

void prvTxTask(void *pvParameters);
void prvRxTask(void *pvParameters);

void uputc(USART_TypeDef* ch,char c);
void HandleSerialCommand(char* line);

#endif
