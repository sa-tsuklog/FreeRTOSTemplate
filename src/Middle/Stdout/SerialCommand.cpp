#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "SerialCommand.h"
#include "CommandList.h"
#include "Drivers/PeriphLib/USART1.h"
#include "Drivers/PeriphLib/USART2.h"
#include "Drivers/PeriphLib/USART3.h"
#include "App/Logger/Logger.h"
#include "App/Gains/Gains.h"
#include "Middle/Gps/Gps.h"

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
		printf("\n\r");
		return;
	}
	
	
	while(commandList[i].command != NULL){
		if(strncmp(line,commandList[i].command,strlen(commandList[i].command))==0){
			(commandList[i].function)( line+strlen(commandList[i].command) );
			return;
		}
		i++;
	}
	printf("invalid command:%s\n\r",line);
	
	
//	if(strncmp(line,"vTaskList",9)==0){
//		char buf[768];
//		vTaskList(buf);
//		printf("task name\tstat\tprirty\tstack\ttasknum\n\r");
//		printf(buf);
//	}else{
//		printf("invalid command:%s\n\r",line);
//	}
}

void SerialCommand::printHelp(){
	int i=0;
	while(commandList[i].command != NULL){
		printf("%s\n\r",commandList[i].command);
		i++;
	}
}
void SerialCommand::printTaskList(){
	vTaskList(vTaskListBuf);
	printf("task name\tstat\tprirty\tstack\ttasknum\n\r");
	printf(vTaskListBuf);
}
void SerialCommand::startLogging(char* arg){
	if(arg[0] == NULL){
		printf("usage: startLogging filename\n\r");
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
void SerialCommand::gpsInvalidate(){
	Gps::GetInstance()->setGpsValidate(0);
}
void SerialCommand::gpsValidate(){
	Gps::GetInstance()->setGpsValidate(1);
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
