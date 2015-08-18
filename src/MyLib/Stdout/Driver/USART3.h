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
#include "semphr.h"

#include "../IoBaseType.h"

class USART3Class :public IoBaseType{
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
	static const int TX_BUFFERSIZE = 768;
	static const int RX_BUFFERSIZE = 768;
	static const int LINE_BUF_SIZE = 768;
	
	SemaphoreHandle_t txCompleteSem;
	xQueueHandle m_queue3;
	char m_txBuf[TX_BUFFERSIZE];
	char m_rxBuf[RX_BUFFERSIZE];
	char m_lineBuf[LINE_BUF_SIZE];
	int rxBufIndex;
	
	int echo;
public:
	virtual void Tx();	//length must be shorter than TX_BUFFERSIZE
	//virtual void Rx();
	virtual char* readLine();
	virtual char getChar();
	virtual void setEcho(int newValue){echo = newValue;};
	xQueueHandle getTxQueue(){ return m_queue3; }

	// Task definition
public:
	static void prvTxTask(void *pvParameters);
	static void prvRxTask(void *pvParameters);
	
	void myUSART3_IRQHandler();
	void myDMA1_Stream4IRQHandler();
};


#endif /* USART3_HPP_ */
