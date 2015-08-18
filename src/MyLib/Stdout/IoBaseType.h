/*
 * IoBaseType.h
 *
 *  Created on: 2015/08/10
 *      Author: sa
 */

#ifndef IOBASETYPE_H_
#define IOBASETYPE_H_

#include "FreeRTOS.h"
#include "queue.h"

class IoBaseType{
public:
	virtual char* readLine();
	virtual char getChar();
	virtual xQueueHandle getTxQueue();
	virtual void myUSART3_IRQHandler();
	virtual void myDMA1_Stream4IRQHandler();
	virtual void setEcho(int newValue);
};

#endif /* IOBASETYPE_H_ */
