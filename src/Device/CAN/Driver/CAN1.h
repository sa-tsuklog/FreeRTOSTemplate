/*
 * BxCAN.h
 *
 *  Created on: 2015/03/13
 *      Author: sa
 */

#ifndef CAN1_H_
#define CAN1_H_

#include "stm32f4xx_can.h"

class CAN1Class {
	// Singleton pattern definition
private:
	CAN1Class();
	CAN1Class(const CAN1Class& rhs);
	CAN1Class& operator=(const CAN1Class& rhs);
	virtual ~CAN1Class() {}
public:
	static CAN1Class* GetInstance() {
		static CAN1Class instance;
		return &instance;
	}

	// Class definition
private:
	
public:
	void send(CanTxMsg msg);
	uint32_t getFifoCount();
	CanRxMsg receive();
	
};

#endif /* CAN1_H_ */
