/*
 * TIM3.h
 *
 *  Created on: 2014/04/21
 *      Author: sa
 */

#ifndef TIM3_H_
#define TIM3_H_


class ServoTim3 {
	// Singleton pattern definition
private:
	ServoTim3();
	ServoTim3(const ServoTim3& rhs);
	ServoTim3& operator=(const ServoTim3& rhs);
	virtual ~ServoTim3() {}
public:
	static ServoTim3* GetInstance() {
    	static ServoTim3 instance;
    	return &instance;
	}

	// Class definition
private:
public:
	void timerStart();
	void setDuty(int ch,uint32_t duty);
};

#endif /* TIM3_H_ */
