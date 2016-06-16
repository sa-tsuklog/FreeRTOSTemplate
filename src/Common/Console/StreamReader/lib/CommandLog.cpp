/*
 * CommandLog.cpp
 *
 *  Created on: 2016/04/05
 *      Author: sa
 */

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "CommandLog.h"

CommandLog::CommandLog(){
	
}
CommandLog::CommandLog(int32_t maxCommandLength,int32_t logDepth){
	this->maxCommandLength = maxCommandLength;
	this->logDepth = logDepth;
	
	uint8_t* commandBuffer = (uint8_t*)malloc(sizeof(uint8_t)*maxCommandLength*logDepth);
	for(int i=0;i<maxCommandLength*logDepth;i++){
		commandBuffer[i] = 0;
	}
	
	commandLogArray = (uint8_t**)malloc(sizeof(uint8_t*)*logDepth);
	
	for(int i=0;i<logDepth;i++){
		commandLogArray[i] = &(commandBuffer[i*maxCommandLength]);
	}
	
	appendPos = 0;
	viewPos = 0;
}
uint8_t* CommandLog::getPreviousCommand(const uint8_t* currentCommand){
	//if the buffer equals newest log, store current command
	if(viewPos == appendPos){
		int length = strlen((const char*)currentCommand);
		for(int i=0;i<length;i++){
			commandLogArray[viewPos][i] = currentCommand[i]; 
		}
		commandLogArray[viewPos][length] = 0;
	}
	
	
	int32_t tmpViewPos = (viewPos-1>=0) ? viewPos-1 : viewPos-1+logDepth;
	
	//if reached to oldest log, don't go back anymore
	if(commandLogArray[tmpViewPos][0] == 0){
		return commandLogArray[viewPos];
	}
		
	//don't wrap around.
	if(tmpViewPos == appendPos){
		return commandLogArray[viewPos];
	}else{
		viewPos = tmpViewPos;
		return commandLogArray[viewPos];
	}
	
	
}
uint8_t* CommandLog::getNextCommand(const uint8_t* currentCommand){
	if(viewPos != appendPos){
		viewPos = (viewPos+1)%logDepth;
		
		return commandLogArray[viewPos];
	}else{
		return currentCommand;
	}
}
void CommandLog::appendCommandList(const uint8_t* command){
	//store last command to buffer.
	int i=0;
	for(i=0;command[i]!=0 && i<maxCommandLength-1;i++){
		commandLogArray[appendPos][i]=command[i];
	}
	commandLogArray[appendPos][i] = 0;
	
	//update index.
	appendPos = (appendPos+1)%logDepth;
	viewPos = appendPos;
	
	//clear buffer for new command.
	commandLogArray[appendPos][0] = 0;
	
	
}
