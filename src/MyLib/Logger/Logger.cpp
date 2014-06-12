/*
 * Logger.cpp
 *
 *  Created on: 2014/05/29
 *      Author: sa
 */

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "GeneralConfig.h"

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
	
	filenameQueue = xQueueCreate(MAX_FILENAME_LENGTH,sizeof(char));
}

void Logger::startLogging(char* filename){
	for(int i=0;i<MAX_FILENAME_LENGTH;i++){
		if(filename[i] == 0){
			break;
		}
		xQueueSendToBack(filenameQueue,&(filename[i]),0);
	}
	xSemaphoreGive(startLoggingSem);
}
void Logger::stopLogging(){
	xSemaphoreGive(stopLoggingSem);
}
void Logger::appendLog(int bufNumber,char* string,int len){
	for(int i=0;i<len;i++){
		if(xQueueSend(buf[bufNumber],&(string[i]),0) != pdTRUE){
			if(logState == Logging){
				printf("queue full\n\r");				
			}
			break;
		}
	}
	
//	while(string[i]!=0){
//		if(xQueueSend(buf[bufNumber],&(string[i]),0) != pdTRUE){
//			if(logState == Logging){
//				printf("queue full\n\r");				
//			}
//			break;
//		}
//		i++;
//	}
}

int Logger::fileOpen(){
	int index=0;
	char c;
	while(xQueueReceive(filenameQueue,&c,0)==pdTRUE){
		filenameBuf[index] = c;
		index++;
	}
	filenameBuf[index] = 0;
	
	FILE* tmpFp = fopen(filenameBuf,"w");
	fclose(tmpFp); 
	
	FRESULT res = f_open(&fp,filenameBuf,FA_WRITE|FA_CREATE_ALWAYS);
	
	logState = Logging;
	//if(fp!=NULL)
	if(res == FR_OK){
		return 1;
	}else{
		printf("cannot open file%s\r\n",filenameBuf);
		return 0;
	}
}
int Logger::fileClose(){
	//FILE* tmpFp = fp;
	//fp = NULL;
	//fclose(tmpFp);
	f_close(&fp);
	logState = logIdle;
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
		lineBuf[charCount] = 0;
		//fprintf(fp,lineBuf);
		UINT bw;
		f_write(&fp,lineBuf,charCount,&bw);
	}
	return 1;
}

void Logger::loggerTask(){
	Logger::GetInstance();
	vTaskDelay(MS_INITIAL_DELAY);
	
	while(1){
		if(logState == logIdle){//when not logging
			if(xSemaphoreTake(startLoggingSem,0)==pdTRUE){
				//clearing sem and queues
				for(int i=0;i<4;i++){
					xQueueReset(buf[i]);
				}
				
				fileOpen();
				
				//clearing sem and queues
				xSemaphoreTake(stopLoggingSem,0);
				for(int i=0;i<4;i++){
					xQueueReset(buf[i]);
				}
			}
		}else if(logState == Logging){//when logging.
			for(int i=0;i<4;i++){
				flushBuffers(i);
			}
			
			if(xSemaphoreTake(stopLoggingSem,0)==pdTRUE){
				fileClose();
				//clearing sem and queues
				xSemaphoreTake(startLoggingSem,0);
				xQueueReset(filenameQueue);
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
	xTaskCreate(prvLoggerTask,"log",1024,NULL,1,NULL);
}
