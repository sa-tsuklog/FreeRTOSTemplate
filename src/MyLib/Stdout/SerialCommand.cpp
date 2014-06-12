#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "SerialCommand.h"
#include "CommandList.h"

#include "MyLib/Logger/Logger.h"
#include "MyLib/Stdout/Driver/USART3.h"
#include "MyLib/Gains/Gains.h"
#include "MyLib/Gains/Driver/Gps/Gps.h"


#ifdef __cplusplus
extern "C" {
#endif

char* vTaskListBuf;

SerialCommand::SerialCommand(){
	vTaskListBuf = malloc(sizeof(char)*768);
}

void SerialCommand::handleSerialCommand(char* line){
	int i=0;
	
	if(strlen(line)==0){
		printf("\r\n");
		return;
	}
	
	
	while(commandList[i].command != NULL){
		if(strncmp(line,commandList[i].command,strlen(commandList[i].command))==0){
			(commandList[i].function)( line+strlen(commandList[i].command) );
			return;
		}
		i++;
	}
	printf("invalid command:%s\r\n",line);
	
	
//	if(strncmp(line,"vTaskList",9)==0){
//		char buf[768];
//		vTaskList(buf);
//		printf("task name\tstat\tprirty\tstack\ttasknum\r\n");
//		printf(buf);
//	}else{
//		printf("invalid command:%s\r\n",line);
//	}
}

void SerialCommand::printHelp(){
	int i=0;
	while(commandList[i].command != NULL){
		printf("%s\r\n",commandList[i].command);
		i++;
	}
}
void SerialCommand::printTaskList(){
	vTaskList(vTaskListBuf);
	printf("task name\tstat\tprirty\tstack\ttasknum\r\n");
	printf(vTaskListBuf);
}
void SerialCommand::startLogging(char* arg){
	if(arg[0] == NULL){
		printf("usage: startLogging filename\r\n");
		return;
	}
	
	Logger::GetInstance()->startLogging(arg+1);
	
}
void SerialCommand::stopLogging(){
	Logger::GetInstance()->stopLogging();
}
void SerialCommand::resetImu(){
	Gains::GetInstance()->resetImu();
}
void SerialCommand::resetGpsRef(){
	Gps::GetInstance()->resetRefPosition();
}
void SerialCommand::echoOn(){
	USART3Class::GetInstance()->setEcho(1);
}
void SerialCommand::echoOff(){
	USART3Class::GetInstance()->setEcho(0);
}

#ifdef __cplusplus
}
#endif
