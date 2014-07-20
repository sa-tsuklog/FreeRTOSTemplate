/*
 * TIM5.h
 *
 *  Created on: 2014/04/21
 *      Author: sa
 */

#ifndef TIM5_H_
#define TIM5_H_


class ServoTim5 {
	// Singleton pattern definition
private:
	ServoTim5();
	ServoTim5(const ServoTim5& rhs);
	ServoTim5& operator=(const ServoTim5& rhs);
	virtual ~ServoTim5() {}
public:
	static ServoTim5* GetInstance() {
    	static ServoTim5 instance;
    	return &instance;
	}

	// Class definition
private:
public:
	void timerStart();
	void setDuty(int ch,uint32_t duty);
};

#endif /* TIM5_H_ */
