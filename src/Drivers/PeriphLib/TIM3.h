/*
 * HalTim.h
 *
 *  Created on: 2014/02/27
 *      Author: sa
 */

#ifndef HALTIM_H_
#define HALTIM_H_

class TIM3Class {
	// Singleton pattern definition
private:
	TIM3Class();
	TIM3Class(const TIM3Class& rhs);
	TIM3Class& operator=(const TIM3Class& rhs);
	virtual ~TIM3Class() {}
public:
	static TIM3Class* GetInstance() {
    	static TIM3Class instance;
    	return &instance;
	}

	// Class definition
private:
public:
};


#endif /* HALTIM_H_ */
