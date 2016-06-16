/*
 * EscapeSequenceParser.h
 *
 *  Created on: 2016/06/15
 *      Author: sa
 */

#ifndef ESCAPESEQUENCEREADER_H_
#define ESCAPESEQUENCEREADER_H_

#include "../Interfaces/StreamReader.h"

class EscapeSequenceReader:public StreamReader{
private:
	StreamReader* parentReader;
	
public:
	EscapeSequenceReader(StreamReader* parentReader);
	~EscapeSequenceReader(){};
	uint32_t getChar();
	void setEcho(uint32_t newValue);
};

#endif /* ESCAPESEQUENCEPARSER_H_ */
