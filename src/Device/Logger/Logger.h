/*
 * Logger.h
 *
 *  Created on: 2014/05/29
 *      Author: sa
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "ff.h"

typedef enum{
	logIdle,
	Logging,
}LogState;


class Logger{
	// Singleton pattern definition
private:
	Logger();
	Logger(const Logger& rhs);
	Logger& operator=(const Logger& rhs);
	virtual ~Logger() {}
public:
	static Logger* GetInstance() {
    	static Logger instance;
    	return &instance;
	}
	
	
	
private:
	TaskHandle_t logHandle;
	
	FIL fp;
	LogState logState;
	
	static const int MAX_FILENAME_LENGTH = 32;
	QueueHandle_t filenameQueue;
	char filenameBuf[MAX_FILENAME_LENGTH+1];
	SemaphoreHandle_t startLoggingSem;
	SemaphoreHandle_t stopLoggingSem;
	
	static const int BUF0_LENGTH = 4096;
	static const int BUF1_LENGTH = 1;
	static const int BUF2_LENGTH = 1;
	static const int BUF3_LENGTH = 1;	
	QueueHandle_t buf[4];
	
	char lineBuf[BUF0_LENGTH];
	
	int fileOpen();
	int fileClose();
	int flushBuffers(int bufNumber);
	
	void loggerTask();
public:
	void startLogging(char* filename);
	void stopLogging();
	void appendLog(int bufNumber,char* string,int len);
	
	static void prvLoggerTask(void* pvParamters);
	static void initLogger();
};


#endif /* LOGGER_H_ */
