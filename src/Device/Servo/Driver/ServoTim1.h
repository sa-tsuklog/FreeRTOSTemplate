/*
 * TIM1.h
 *
 *  Created on: 2014/04/21
 *      Author: sa
 */

#ifndef TIM1_H_
#define TIM1_H_

#include <stdint.h>

class ServoTim1 {
	// Singleton pattern definition
private:
	ServoTim1();
	ServoTim1(const ServoTim1& rhs);
	ServoTim1& operator=(const ServoTim1& rhs);
	virtual ~ServoTim1() {}
public:
	static ServoTim1* GetInstance() {
    	static ServoTim1 instance;
    	return &instance;
	}

	// Class definition
private:
public:
	void timerStart();
	void setDuty(int ch,uint32_t duty);
};


#endif /* TIM1_H_ */
