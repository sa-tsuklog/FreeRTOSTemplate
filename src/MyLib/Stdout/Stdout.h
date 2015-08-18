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

#include "IoBaseType.h"

class Stdout {
	// Singleton pattern definition
private:
	Stdout();
	Stdout(const Stdout& rhs);
	Stdout& operator=(const Stdout& rhs);
	virtual ~Stdout() {}
public:
	static Stdout* GetInstance() {
		static Stdout instance;
		return &instance;
	}

	// Class definition
private:
	IoBaseType* io;
	
	TaskHandle_t u3txHandle;
	TaskHandle_t u3rxHandle;
	TaskHandle_t cmdHandle;
public:
	
	char* readLine();
	char getChar();
	xQueueHandle getTxQueue();
	void setEcho(int newValue);
	static void initStdout();	
	
	void myUSART3IRQHandler();
	void myDma1_Stream4IRQHandler();
};


#endif /* STDOUT_H_ */
