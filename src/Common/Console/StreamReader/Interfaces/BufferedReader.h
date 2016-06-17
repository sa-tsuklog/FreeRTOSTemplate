/*
 * BufferedReader.h
 *
 *  Created on: 2016/06/15
 *      Author: sa
 */

#ifndef BUFFEREDREADER_H_
#define BUFFEREDREADER_H_

#include "stdint.h"
#include "StreamReader.h"

class BufferedReader:public StreamReader{
protected:
	uint32_t echo;
public:
	virtual ~BufferedReader(){};
	virtual uint32_t getChar(uint32_t msBlockTime) = 0;
	virtual uint32_t getChar() = 0;
	virtual uint8_t* readLine() = 0;
	virtual void setEcho(uint32_t newValue){echo=newValue;}
};

#endif /* BUFFEREDREADER_H_ */
