/*
 * BridgeDriver.h
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#ifndef BRIDGEDRIVER_H_
#define BRIDGEDRIVER_H_

class MoterDriver {
	// Singleton pattern definition
private:
	MoterDriver();
	MoterDriver(const MoterDriver& rhs);
	MoterDriver& operator=(const MoterDriver& rhs);
	virtual ~MoterDriver() {}
public:
	static MoterDriver* GetInstance() {
		static MoterDriver instance;
		return &instance;
	}
	
	// Class definition
private:
	static const int USE_CH0 = 1;//marked as "servo 4-7"
	static const int USE_CH1 = 1;//marked as "propo"
	
	static const int MAX_DUTY = (int)(65535*0.90);
public:
	void setPower(int ch,float torque,int direction,int brake);
	
};

#endif /* BRIDGEDRIVER_H_ */
