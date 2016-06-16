/*
 * BufferedReader.h
 *
 *  Created on: 2016/06/15
 *      Author: sa
 */

#ifndef BUFFEREDREADER_H_
#define BUFFEREDREADER_H_

#include "StreamReader.h"

class BufferedReader:public StreamReader{
protected:
	uint32_t echo;
public:
	virtual ~BufferedReader(){};
	virtual uint8_t* readLine() = 0;
	virtual void setEcho(uint32_t newValue){echo=newValue;}
};

#endif /* BUFFEREDREADER_H_ */
