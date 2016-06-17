/*
 * StreamReader.h
 *
 *  Created on: 2016/06/15
 *      Author: sa
 */

#ifndef STREAMREADER_H_
#define STREAMREADER_H_

#include "stdint.h"

class StreamReader{
public:
	StreamReader(){};
	virtual ~StreamReader(){};
	virtual uint32_t getChar(uint32_t msBlockTime) = 0;
	virtual uint32_t getChar() = 0;
};

#endif /* STREAMREADER_H_ */
