#ifndef __HAL_USART_H__
#define __HAL_USART_H__

#include "stm32f4xx.h"

#ifdef __cplusplus
 extern "C" {
#endif

void uputc(USART_TypeDef* ch,char c);

void HandleSerialCommand(char* line);

#ifdef __cplusplus
}
#endif


#endif
