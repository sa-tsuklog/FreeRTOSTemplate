/*
 * EditableLineReader.h
 *
 *  Created on: 2016/06/16
 *      Author: sa
 */

#ifndef EDITABLELINEREADER_H_
#define EDITABLELINEREADER_H_

#include "Interfaces/BufferedReader.h"
#include "Interfaces/StreamReader.h"
#include "lib/CommandLog.h"

class EditableLineReader:public BufferedReader{
private:
	StreamReader* reader;
	CommandLog commandLog;
	uint8_t* lineBuf;
	uint32_t lineLength;
	
	FILE* echoFp;
	
	void printArrowLeft();
	void printArrowRight();
	void printLineClear();
	void printNumberDititInAscii(uint32_t num);
	void printNumberInAscii(uint32_t num);
	void printMoveCursorTo(uint32_t cursorPos);
public:
	EditableLineReader(StreamReader* reader,uint32_t lineLength,uint32_t logDepth,FILE* echoFp);
	~EditableLineReader();
	
	uint32_t getChar();
	uint8_t* readLine();
};

#endif /* EDITABLELINEREADER_H_ */
