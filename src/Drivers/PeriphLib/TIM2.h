/*
 * TIM2.h
 *
 *  Created on: 2014/04/20
 *      Author: sa
 */

#ifndef TIM2_H_
#define TIM2_H_

class TIM2Class {
	// Singleton pattern definition
private:
	TIM2Class();
	TIM2Class(const TIM2Class& rhs);
	TIM2Class& operator=(const TIM2Class& rhs);
	virtual ~TIM2Class() {}
public:
	static TIM2Class* GetInstance() {
    	static TIM2Class instance;
    	return &instance;
	}

	// Class definition
private:
public:
	void timerStart();
	int getUsCycle();
	int getUsDuty();
};

void prvTIM2TestTask(void* pvParameters);

#endif /* TIM2_H_ */
