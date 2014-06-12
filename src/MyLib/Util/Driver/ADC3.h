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
	static const float VOLT_PER_LSB = 3.3*11/0xFFF; 
public:
	void startFirstConversion(); 
	float getNextData();
};

#endif
