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

class USART1Class {
	// Singleton pattern definition
private:
	USART1Class();
	USART1Class(const USART1Class& rhs);
	USART1Class& operator=(const USART1Class& rhs);
	virtual ~USART1Class() {}
public:
	static USART1Class* GetInstance() {
    	static USART1Class instance;
    	return &instance;
	}

	// Class definition
private:
	static const int TX_BUFFERSIZE = 1024;
	static const int RX_BUFFERSIZE = 64;
	static const int LINE_BUF_SIZE = 64;
	xQueueHandle m_queue1;
	char m_txBuf[TX_BUFFERSIZE];
	char m_rxBuf[RX_BUFFERSIZE];
	char m_lineBuf[LINE_BUF_SIZE];
public:
	virtual void Tx();
	virtual void Rx();
	xQueueHandle GetQueue(){ return m_queue1; }

	// Task definition
public:
	static void prvTxTask(void *pvParameters);
	static void prvRxTask(void *pvParameters);
};


#endif /* USART1_H_ */
