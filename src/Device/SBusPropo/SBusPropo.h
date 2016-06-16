/*
 * SBusPropo.h
 *
 *  Created on: 2015/08/12
 *      Author: sa
 */

#ifndef SBUSPROPO_H_
#define SBUSPROPO_H_

#include "FreeRTOS.h"
#include "queue.h"

/////////////////////////////////////
// assume 2ail or 1ail type control.
// SWB is available only 1ail control.
/////////////////////////////////////

class SBusPropo {
	// Singleton pattern definition
private:
	SBusPropo();
	SBusPropo(const SBusPropo& rhs);
	SBusPropo& operator=(const SBusPropo& rhs);
	virtual ~SBusPropo() {}
public:
	static SBusPropo* GetInstance() {
		static SBusPropo instance;
		return &instance;
	}

	// Class definition
private:
	static const int RX_BUFFERSIZE = 32;
	
	xQueueHandle rxQueue;
	
	uint8_t lineBuffer[RX_BUFFERSIZE];
	
	uint16_t elevator;
	uint16_t aileron;
	uint16_t rudder;
	uint16_t throttle;
	uint16_t swB;
	uint16_t swC;
	int16_t elevatorBias;
	int16_t aileronBias;
	int16_t rudderBias;
	int16_t throttleBottom;
	
	
	
	void SBusPropoTask();
	void test(uint8_t* lineBuffer,int length);
	void decodeMessage(uint8_t* lineBuffer,int length);
public:
	xQueueHandle getRxQueue(){return rxQueue;};
	static void SBusPropoTaskEntry(void *pvParameters);
	static void initSBusPropo();
};

#endif /* SBUSPROPO_H_ */
