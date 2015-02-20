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
	TaskHandle_t u3txHandle;
	TaskHandle_t u3rxHandle;
public:
	static void initStdout();	
};


#endif /* STDOUT_H_ */
