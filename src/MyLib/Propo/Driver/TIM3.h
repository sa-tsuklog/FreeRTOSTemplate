/*
 * HalTim.h
 *
 *  Created on: 2014/02/27
 *      Author: sa
 */

#ifndef HALTIM_H_
#define HALTIM_H_

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
};


#endif /* HALTIM_H_ */
