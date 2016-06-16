/*
 * Stdout.h
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#ifndef STDOUT_H_
#define STDOUT_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "StdoutConfig.h"
#include "Common/Console/StreamReader/Interfaces/BufferedReader.h"
#include "Common/Console/StreamWriter/Interfaces/StreamWriter.h"

class Stdout {
	// Singleton pattern definition
private:
	Stdout();
	Stdout(const Stdout& rhs);
	Stdout& operator=(const Stdout& rhs);
	virtual ~Stdout();
public:
	static Stdout* GetInstance() {
		static Stdout instance;
		return &instance;
	}

	// Class definition
private:
	TaskHandle_t u3txHandle;
	TaskHandle_t cmdHandle;
	
	StreamReader* escReader;
	BufferedReader* reader;
	StreamWriter* writer;
public:
	
	uint8_t* readLine();
	uint8_t getChar();
	xQueueHandle getTxQueue();
	void putChar(uint8_t c);
	void setEcho(int newValue);
	static void initStdout();	
};


#endif /* STDOUT_H_ */

