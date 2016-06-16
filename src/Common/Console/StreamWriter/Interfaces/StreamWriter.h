/*
 * StreamWriter.cpp
 *
 *  Created on: 2016/06/15
 *      Author: sa
 */

#ifndef STREAMWRITER_H_
#define STREAMWRITER_H_

#include "FreeRTOS.h"
#include "queue"

class StreamWriter{
public:
	StreamWriter(){};
	virtual ~StreamWriter(){};
	virtual void putChar(uint32_t c) = 0;
	virtual QueueHandle_t getTxQueue() = 0;
};


#endif /* STREAMWRITER_H_ */
