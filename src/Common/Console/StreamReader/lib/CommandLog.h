/*
 * CommandLog.h
 *
 *  Created on: 2016/04/05
 *      Author: sa
 */

#ifndef COMMANDLOG_H_
#define COMMANDLOG_H_

#include "stdint.h"

class CommandLog{
private:
	
	int32_t maxCommandLength;
	int32_t logDepth;
	
	uint8_t** commandLogArray;
	
	int32_t appendPos;
	int32_t viewPos;
public:
	CommandLog();
	CommandLog(int32_t maxCommandLength,int32_t logDepth);
	uint8_t* getPreviousCommand(const uint8_t* currentCommand);
	uint8_t* getNextCommand(const uint8_t* currentCommand);
	void appendCommandList(const uint8_t* command);
	
};

#endif /* COMMANDLOG_H_ */
