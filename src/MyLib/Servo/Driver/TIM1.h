/*
 * TIM1.h
 *
 *  Created on: 2014/04/21
 *      Author: sa
 */

#ifndef TIM1_H_
#define TIM1_H_

class TIM1Class {
	// Singleton pattern definition
private:
	TIM1Class();
	TIM1Class(const TIM1Class& rhs);
	TIM1Class& operator=(const TIM1Class& rhs);
	virtual ~TIM1Class() {}
public:
	static TIM1Class* GetInstance() {
    	static TIM1Class instance;
    	return &instance;
	}

	// Class definition
private:
public:
	void timerStart();
};


#endif /* TIM1_H_ */
