/*
 * USART3_CMM920.hpp
 *
 *  Created on: 2014/04/27
 *      Author: sa
 */

#ifndef USART3_CMM920_HPP_
#define USART3_CMM920_HPP_

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "UsartCmm920Config.h"

class USART3_CMM920Class {
	// Singleton pattern definition
private:
	USART3_CMM920Class();
	USART3_CMM920Class(const USART3_CMM920Class& rhs);
	USART3_CMM920Class& operator=(const USART3_CMM920Class& rhs);
	virtual ~USART3_CMM920Class(){};
public:
	static USART3_CMM920Class* GetInstance() {
    	static USART3_CMM920Class instance;
    	return &instance;
	}

	// Class definition
private:
	SemaphoreHandle_t txCompleteSem;
private:
public:
	virtual void Tx(unsigned char* message,int length);
public:
	void myUSART3_IRQHandler();
	void myDMA1_Stream4IRQHandler();
};


#endif /* USART3_CMM920_HPP_ */
