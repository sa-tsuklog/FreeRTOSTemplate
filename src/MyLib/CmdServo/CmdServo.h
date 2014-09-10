/*
 * CmdServo.h
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#ifndef CMDSERVO_H_
#define CMDSERVO_H_

#include "Driver/USART1.h"

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
	
	
	void sendShortPacket(unsigned char id,unsigned char flag,unsigned char address,unsigned char length,unsigned char* data);
public:
	USART1Class* usart;
	
	void setMargin(unsigned char id,unsigned char mdeg_x100Margin,int isCw);
	void setSlope(unsigned char id,unsigned char degSlope,int isCw);
	void setPunch(unsigned char id,unsigned short percentPunch);
	
	void on(unsigned char id);
	void setPos(unsigned char id,float degPos);
	void setServoId(unsigned char oldId, unsigned char newId);
	void setAngleLimit(unsigned char id,float degLimit);
	void setMoveTime(unsigned char id,unsigned int ms_x10MoveTime);
	void setTorqueLimit(unsigned char id,unsigned char percentTorqueLimit);
	void flashFinalize(unsigned char id);
	
	static void initCmdServo();
};

#endif /* CMDSERVO_H_ */
