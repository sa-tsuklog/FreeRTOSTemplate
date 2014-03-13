#ifndef __HAL_USART_H__
#define __HAL_USART_H__

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "queue.h"

class USART2Class {
	// Singleton pattern definition
private:
	USART2Class();
	USART2Class(const USART2Class& rhs);
	USART2Class& operator=(const USART2Class& rhs);
	virtual ~USART2Class() {}
public:
	static USART2Class* GetInstance() {
    	static USART2Class instance;
    	return &instance;
	}

	// Class definition
private:
	static const int TX_BUFFERSIZE = 1024;
	static const int RX_BUFFERSIZE = 64;
	static const int LINE_BUF_SIZE = 64;
	xQueueHandle m_queue;
	char m_txBuf[TX_BUFFERSIZE];
	char m_rxBuf[RX_BUFFERSIZE];
	char m_lineBuf[LINE_BUF_SIZE];
public:
	void Tx();
	void Rx();
	xQueueHandle GetQueue(){ return m_queue; }
};

void prvTxTask(void *pvParameters);
void prvRxTask(void *pvParameters);

void uputc(USART_TypeDef* ch,char c);
void callbackUsart2CrReceived(char* line);

#endif
