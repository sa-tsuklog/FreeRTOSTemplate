#ifndef __ADC3_H_
#define __ADC3_H_

#include "stm32f4xx.h"

class ADC1Class {
	// Singleton pattern definition
private:
	ADC1Class();
	ADC1Class(const ADC1Class& rhs);
	ADC1Class& operator=(const ADC1Class& rhs);
	virtual ~ADC1Class() {}
public:
	static ADC1Class* GetInstance() {
    	static ADC1Class instance;
    	return &instance;
	}

	// Class definition
private:
	uint16_t m_value[2];
public:
	uint16_t GetData(int pinNum);

	// Task definition
public:
	static void prvTask(void *pvParameters);
};

#endif
