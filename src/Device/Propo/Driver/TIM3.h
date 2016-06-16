/*
 * HalTim.h
 *
 *  Created on: 2014/02/27
 *      Author: sa
 */

#ifndef HALTIM_H_
#define HALTIM_H_

class PropoTim3 {
	// Singleton pattern definition
private:
	PropoTim3();
	PropoTim3(const PropoTim3& rhs);
	PropoTim3& operator=(const PropoTim3& rhs);
	virtual ~PropoTim3() {}
public:
	static PropoTim3* GetInstance() {
    	static PropoTim3 instance;
    	return &instance;
	}

	// Class definition
private:
public:
	void timerStart();
	int getUsTime(int ch);
};


#endif /* HALTIM_H_ */
