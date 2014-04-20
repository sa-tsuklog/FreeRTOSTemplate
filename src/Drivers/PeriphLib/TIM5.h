/*
 * TIM5.h
 *
 *  Created on: 2014/04/20
 *      Author: sa
 */

#ifndef TIM5_H_
#define TIM5_H_

class TIM5Class {
	// Singleton pattern definition
private:
	TIM5Class();
	TIM5Class(const TIM5Class& rhs);
	TIM5Class& operator=(const TIM5Class& rhs);
	virtual ~TIM5Class() {}
public:
	static TIM5Class* GetInstance() {
    	static TIM5Class instance;
    	return &instance;
	}

	// Class definition
private:
public:
	void timerStart();
};

void prvTIM5TestTask(void* pvParameters);

#endif /* TIM5_H_ */
