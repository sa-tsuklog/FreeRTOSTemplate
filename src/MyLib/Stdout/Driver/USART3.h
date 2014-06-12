/*
 * USART3.hpp
 *
 *  Created on: 2014/04/27
 *      Author: sa
 */

#ifndef USART3_HPP_
#define USART3_HPP_

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "queue.h"

class USART3Class {
	// Singleton pattern definition
private:
	USART3Class();
	USART3Class(const USART3Class& rhs);
	USART3Class& operator=(const USART3Class& rhs);
	virtual ~USART3Class() {}
public:
	static USART3Class* GetInstance() {
    	static USART3Class instance;
    	return &instance;
	}

	// Class definition
private:
	static const int TX_BUFFERSIZE = 1024;
	static const int RX_BUFFERSIZE = 64;
	static const int LINE_BUF_SIZE = 64;
	xQueueHandle m_queue3;
	char m_txBuf[TX_BUFFERSIZE];
	char m_rxBuf[RX_BUFFERSIZE];
	char m_lineBuf[LINE_BUF_SIZE];
	
	int echo;
public:
	virtual void Tx();
	virtual void Rx();
	virtual void setEcho(int newValue){echo = newValue;};
	xQueueHandle GetQueue(){ return m_queue3; }

	// Task definition
public:
	static void prvTxTask(void *pvParameters);
	static void prvRxTask(void *pvParameters);
};


#endif /* USART3_HPP_ */
