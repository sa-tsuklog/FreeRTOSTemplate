/*
 * BridgeTim1.h
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#ifndef BRIDGETIM1_H_
#define BRIDGETIM1_H_

#include "BridgeTim.h"

class BridgeTim1:public BridgeTim{
	// Singleton pattern definition
private:
	BridgeTim1();
	BridgeTim1(const BridgeTim1& rhs);
	BridgeTim1& operator=(const BridgeTim1& rhs);
	virtual ~BridgeTim1() {}
public:
	static BridgeTim1* GetInstance() {
		static BridgeTim1 instance;
		return &instance;
	}

	// Class definition
private:
	
public:
	void setDuty(int ch,unsigned short duty);
	void timerStart();
};

#endif /* BRIDGETIM1_H_ */
