/*
 * Logger.cpp
 *
 *  Created on: 2014/05/29
 *      Author: sa
 */
#include <pch.h>

#include "Device/Util/Util.h"
#include "Logger.h"
#include "OS/MyFileHandle.h"


Logger::Logger(){
	logState = logIdle;
	
	startLoggingSem = xSemaphoreCreateBinary();
	stopLoggingSem = xSemaphoreCreateBinary();
	
	xSemaphoreTake(startLoggingSem,0);
	xSemaphoreTake(stopLoggingSem,0);
	
	buf[0] = xQueueCreate(BUF0_LENGTH,sizeof(char));
	buf[1] = xQueueCreate(BUF1_LENGTH,sizeof(char));
	buf[2] = xQueueCreate(BUF2_LENGTH,sizeof(char));
	buf[3] = xQueueCreate(BUF3_LENGTH,sizeof(char));
	vQueueAddToRegistry(buf[0],"log0");
	vQueueAddToRegistry(buf[1],"log1");
	vQueueAddToRegistry(buf[2],"log2");
	vQueueAddToRegistry(buf[3],"log3");
	
	filenameQueue = xQueueCreate(MAX_FILENAME_LENGTH,sizeof(char));
	vQueueAddToRegistry(filenameQueue,"logFile");
	
	logOverrunOccured = 0;
}

void Logger::startLogging(char* filename){
	if(logState == logIdle){
	for(int i=0;i<MAX_FILENAME_LENGTH;i++){
		if(filename[i] == 0){
			break;
		}
		xQueueSendToBack(filenameQueue,&(filename[i]),0);
	}
		xSemaphoreGive(startLoggingSem);
	}else{
		printf("logging task is already running\r\n");
	}
}
void Logger::stopLogging(){
	xSemaphoreGive(stopLoggingSem);
}
void Logger::appendLog(int bufNumber,char* string,int len){
	if(logState != Logging){
		return;
	}
	if(len > uxQueueSpacesAvailable(buf[bufNumber])){
		//printf("log overrun\r\n");
		logOverrunOccured++;
		return;
	}
	
	for(int i=0;i<len;i++){
		if(xQueueSend(buf[bufNumber],&(string[i]),0) != pdTRUE){
			//printf("log overrun\r\n");
			logOverrunOccured++;
			break;
		}
	}
}

int Logger::fileOpen(){
	int index = 0;
	char c;
	while(xQueueReceive(filenameQueue,&c,0) == pdTRUE){
		filenameBuf[index] = c;
		index++;
	}
	filenameBuf[index] = 0;
	
	fp2 = fopen(filenameBuf,"wb");
	
	if(fp2 != NULL){
		return 1;
	}else{
		return 0;
	}
	
}
int Logger::fileClose(){
	fclose(fp2);
	return 1;
}

int Logger::flushBuffers(int bufNumber){
	int charCount; 
	char c;
	if((charCount = uxQueueMessagesWaiting(buf[bufNumber]))!=0){
		for(int i=0;i<charCount;i++){
			xQueueReceive(buf[bufNumber],&c,0);
			lineBuf[i] = c;
		}
		
		fwrite(lineBuf,sizeof(char),charCount,fp2);
	}
	
	fflush(fp2);
	return 1;
}

void Logger::loggerTask(){
	Logger::GetInstance();
	vTaskDelay(MS_INITIAL_DELAY);
	uint32_t t1,t2;
	
	while(1){
		
		if(logState == logIdle){//when not logging
			if(xSemaphoreTake(startLoggingSem,0)==pdTRUE){
				logOverrunOccured = 0;
				if(fileOpen() == 1){
					//clearing sem and queues
					xSemaphoreTake(stopLoggingSem,0);
					for(int i=0;i<4;i++){
						xQueueReset(buf[i]);
					}
					logState = Logging;
					Util::GetInstance()->myFprintf(0,stdout,"log start\r\n");
				}else{
					Util::GetInstance()->myFprintf(0,stdout,"cannot open file%s\r\n",filenameBuf);
				}
			}
		}
		if(logState == Logging){//when logging.
			for(int i=0;i<4;i++){
				flushBuffers(i);
			}
			
			
			if(xSemaphoreTake(stopLoggingSem,0)==pdTRUE){
				logState = logIdle;
				fileClose();
				//clearing sem and queues
				xSemaphoreTake(startLoggingSem,0);
				xQueueReset(filenameQueue);
				if(logOverrunOccured){
					printf("warning: overrun occured %d times\r\n",logOverrunOccured);
				}
				
			}
		}
		
		vTaskDelay(10);
	}
}

void Logger::prvLoggerTask(void* pvParamters){
	Logger::GetInstance()->loggerTask();
}

void Logger::initLogger(){
	//task create
	xTaskCreate(prvLoggerTask,"log",1024,NULL,1,&(Logger::GetInstance()->logHandle));
}
