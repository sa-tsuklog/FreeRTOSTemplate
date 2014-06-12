/*
 * TIM4.h
 *
 *  Created on: 2014/04/21
 *      Author: sa
 */

#ifndef TIM4_H_
#define TIM4_H_


class TIM4Class {
	// Singleton pattern definition
private:
	TIM4Class();
	TIM4Class(const TIM4Class& rhs);
	TIM4Class& operator=(const TIM4Class& rhs);
	virtual ~TIM4Class() {}
public:
	static TIM4Class* GetInstance() {
    	static TIM4Class instance;
    	return &instance;
	}

	// Class definition
private:
public:
	void timerStart();
};

#endif /* TIM4_H_ */
