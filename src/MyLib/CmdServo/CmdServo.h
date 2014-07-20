/*
 * CmdServo.h
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#ifndef CMDSERVO_H_
#define CMDSERVO_H_

class CmdServo {
	// Singleton pattern definition
private:
	CmdServo();
	CmdServo(const CmdServo& rhs);
	CmdServo& operator=(const CmdServo& rhs);
	virtual ~CmdServo() {}
public:
	static CmdServo* GetInstance() {
		static CmdServo instance;
		return &instance;
	}

	// Class definition
private:
	
public:
	void set();
	
};

#endif /* CMDSERVO_H_ */
