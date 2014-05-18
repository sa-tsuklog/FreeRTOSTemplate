#ifndef __ADC3_H_
#define __ADC3_H_

#include "stm32f4xx.h"

//for voltage monitor
class ADC3Class {
	// Singleton pattern definition
private:
	ADC3Class();
	ADC3Class(const ADC3Class& rhs);
	ADC3Class& operator=(const ADC3Class& rhs);
	virtual ~ADC3Class() {}
public:
	static ADC3Class* GetInstance() {
    	static ADC3Class instance;
    	return &instance;
	}

	// Class definition
private:
public:
	uint16_t getNextData(int pinNum);
	void startFirstConversion(); 
	// Task definition
public:
	static void prvTask(void *pvParameters);
};

#endif
