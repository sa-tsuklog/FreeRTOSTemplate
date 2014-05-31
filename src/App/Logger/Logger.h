/*
 * Logger.h
 *
 *  Created on: 2014/05/29
 *      Author: sa
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "FreeRTOS.h"
#include "semphr.h"

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
	FILE* fp;
	SemaphoreHandle_t fileSem;
public:
	FILE* getFp();
	void sync();
	void startLogging(char* filename);
	void stopLogging();
	void fileSemTake();
	void fileSemGive();
};


#endif /* LOGGER_H_ */
