/*
 * USART1.h
 *
 *  Created on: 2014/04/25
 *      Author: sa
 */

#ifndef USART1_H_
#define USART1_H_

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "queue.h"

class USART1ClassPropo {
	// Singleton pattern definition
private:
	USART1ClassPropo();
	USART1ClassPropo(const USART1ClassPropo& rhs);
	USART1ClassPropo& operator=(const USART1ClassPropo& rhs);
	virtual ~USART1ClassPropo() {}
public:
	static USART1ClassPropo* GetInstance() {
    	static USART1ClassPropo instance;
    	return &instance;
	}

	// Class definition
private:
	
public:
	// Task definition
public:
	void myUSART1IRQHandler();
};


#endif /* USART1_H_ */
