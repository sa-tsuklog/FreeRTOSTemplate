/*
 * Logger.cpp
 *
 *  Created on: 2014/05/29
 *      Author: sa
 */

#include <stdio.h>
#include <stdlib.h>

#include "Logger.h"
#include "../../Middle/Stdout/myFileHandle.h"

Logger::Logger(){
	fp = NULL;
	fileSem = xSemaphoreCreateMutex();
	if(fileSem == NULL){
		printf("sem error\n\r");
	}
	
}

void Logger::startLogging(char* filename){
	if(fp != NULL){
		return;
	}
	fileSemTake();
	fp = fopen(filename,"w");
	fileSemGive();
	if(fp == NULL){
		printf("cannot open file %s\n\r",filename);
	}else{
		printf("start logging to: %s\n\r",filename);
	}
}
void Logger::stopLogging(){
	if(fp == NULL){
		return;
	}
	fileSemTake();
	fclose(fp);
	fp = NULL;
	fileSemGive();
	
	printf("stop logging\n\r");
	
	
}
FILE* Logger::getFp(){
	return fp;
}
void Logger::sync(){
	fileSemTake();
	myFsync(fp);
	fileSemGive();
}
void Logger::fileSemTake(){
	xSemaphoreTake(fileSem,portMAX_DELAY);
}
void Logger::fileSemGive(){
	xSemaphoreGive(fileSem);
}
