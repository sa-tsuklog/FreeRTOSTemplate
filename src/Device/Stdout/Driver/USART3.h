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

#include "../StdoutConfig.h"

#include "Common/Console/StreamReader/Interfaces/StreamReader.h"
#include "Common/Console/StreamWriter/Interfaces/StreamWriter.h"

class USART3Class :public StreamReader,public StreamWriter{
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
	SemaphoreHandle_t txCompleteSem;
	xQueueHandle txQueue;
	xQueueHandle rxQueue;
	
	char m_txBuf[StdoutConfig::TX_BUFFERSIZE];
public:
	virtual void Tx();	//length must be shorter than TX_BUFFERSIZE
	virtual uint32_t getChar(uint32_t msBlockTime);
	virtual uint32_t getChar();
	virtual void putChar(uint32_t c);
	xQueueHandle getTxQueue(){ return txQueue; }

	// Task definition
public:
	static void prvTxTask(void *pvParameters);
	static void prvRxTask(void *pvParameters);
	
	void myUSART3_IRQHandler();
	void myDMA1_Stream4IRQHandler();
};


#endif /* USART3_HPP_ */
