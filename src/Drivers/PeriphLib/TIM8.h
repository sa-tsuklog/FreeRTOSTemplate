/*
 * TIM8.h
 *
 *  Created on: 2014/04/21
 *      Author: sa
 */

#ifndef TIM8_H_
#define TIM8_H_

class TIM8Class {
	// Singleton pattern definition
private:
	TIM8Class();
	TIM8Class(const TIM8Class& rhs);
	TIM8Class& operator=(const TIM8Class& rhs);
	virtual ~TIM8Class() {}
public:
	static TIM8Class* GetInstance() {
    	static TIM8Class instance;
    	return &instance;
	}

	// Class definition
private:
public:
	void timerStart();
};


#endif /* TIM8_H_ */
