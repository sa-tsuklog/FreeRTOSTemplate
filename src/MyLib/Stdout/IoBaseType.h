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
	virtual char* readLine()=0;
	virtual char getChar()=0;
	virtual xQueueHandle getTxQueue()=0;
	virtual void myUSART3_IRQHandler()=0;
	virtual void myDMA1_Stream4IRQHandler()=0;
	virtual void setEcho(int newValue)=0;
};

#endif /* IOBASETYPE_H_ */
