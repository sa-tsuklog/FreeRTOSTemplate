/*
 * BridgeTim3.h
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#ifndef BRIDGETIM3_H_
#define BRIDGETIM3_H_

class BridgeTim3 {
	// Singleton pattern definition
private:
	BridgeTim3();
	BridgeTim3(const BridgeTim3& rhs);
	BridgeTim3& operator=(const BridgeTim3& rhs);
	virtual ~BridgeTim3() {}
public:
	static BridgeTim3* GetInstance() {
		static BridgeTim3 instance;
		return &instance;
	}

	// Class definition
private:
	
public:
	void setDuty(int ch,unsigned short duty);
	void timerStart();
};

#endif /* BRIDGETIM3_H_ */
