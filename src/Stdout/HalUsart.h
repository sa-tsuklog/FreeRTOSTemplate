/*
 * Stdout.hpp
 *
 *  Created on: 2014/02/10
 *      Author: sa
 */

#ifndef HAL_USART_HPP_
#define HAL_USART_HPP_

#include "stm32f4xx.h"

void initUart(USART_TypeDef* ch);

void prvTxTask(void *pvParameters);

void prvRxTask(void *pvParameters);

void uputc(USART_TypeDef* ch,char c);

void callbackUsart2CrReceived(char* line);




#endif /* STDOUT_HPP_ */
