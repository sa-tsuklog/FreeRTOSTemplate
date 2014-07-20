/*
 * TIM4.h
 *
 *  Created on: 2014/04/21
 *      Author: sa
 */

#ifndef TIM4_H_
#define TIM4_H_


class ServoTim4 {
	// Singleton pattern definition
private:
	ServoTim4();
	ServoTim4(const ServoTim4& rhs);
	ServoTim4& operator=(const ServoTim4& rhs);
	virtual ~ServoTim4() {}
public:
	static ServoTim4* GetInstance() {
    	static ServoTim4 instance;
    	return &instance;
	}

	// Class definition
private:
public:
	void timerStart();
	void setDuty(int ch,uint32_t duty);
};

#endif /* TIM4_H_ */
