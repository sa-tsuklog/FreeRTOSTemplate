/*
 * Cmm920.h
 *
 *  Created on: 2015/08/10
 *      Author: sa
 */

#ifndef CMM920_H_
#define CMM920_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "../IoBaseType.h"
#include "USART3_CMM920.h"
#include "UsartCmm920Config.h"

class GainsPrintMode{
public:
	enum Mode{
		NONE,
		MARITIME,
		E_FRAME,
		DEBUG,
		INS,
		QUATERNION,
		GPAIO,
	};
};

class DecodeState{
public:
	enum State{
		DLE1,
		STX,
		DATALEN1,
		DATALEN2,
		CMD1,
		CMD2,
		RESULT,
		RESULT_DETAIL,
		DATA,
		DLE2,
		ETX,
	};
};

class Cmm920 :public IoBaseType{
	// Singleton pattern definition
private:
	Cmm920();
	Cmm920(const Cmm920& rhs);
	Cmm920& operator=(const Cmm920& rhs);
	virtual ~Cmm920() {}
public:
	static Cmm920* GetInstance() {
		static Cmm920 instance;
		return &instance;
	}

	// Class definition
private:
	//Tx variables and functions
	uint8_t txFrameBuf[UsartCmm920Config::TX_BUFFERSIZE];
	xQueueHandle txQueue;
	SemaphoreHandle_t messageSendCompleteSem;
	
	void Cmm920TxTask();
	
	void setConfigMode();
	void setSendMode();
	void setRfParameter();
	void setAddress(uint16_t panId,uint64_t longAddress,uint16_t shortAddress);
	void requestReset();
	void sendCommand(uint16_t cmd,uint8_t* data,int dataLength);
	void sendMessage(xQueueHandle messageQueue,int messageLength);
	void constructTxHeader(uint16_t cmd,int dataLength);
	void constructTxData(uint8_t* data,int dataLength);
	uint8_t calcChecksum(int dataLength);
	
	//Rx variables and functions
	xQueueHandle rxQueue;
	xQueueHandle rxMessageQueue;
	int echo;
	uint8_t rxSum;
	int rxDataLength;
	uint8_t rxCmd1,rxCmd2;
	uint8_t rxResult,rxResultDetail;
	int rxDataCount;
	uint8_t rxDataBuffer[UsartCmm920Config::RX_BUFFERSIZE];
	uint8_t rxLineBuffer[UsartCmm920Config::LINE_BUFFERSIZE];
	DecodeState::State state;
	
	void Cmm920RxTask();
	
	void decodeCommand(uint8_t buf);
	void decodeData(uint8_t* dataBuffer, int length);
	void decodeMessage(uint8_t* messageBuffer, int length);
	
public:
	char* readLine();
	char getChar();
	xQueueHandle getTxQueue(){ return txQueue; }
	xQueueHandle getRxQueue(){ return rxQueue; }
	
	static void Cmm920TxTaskEntry(void *pvParameters);
	static void Cmm920RxTaskEntry(void *pvParameters);
	
	void myUSART3_IRQHandler();
	void myDMA1_Stream4IRQHandler();
	void setEcho(int newState);
};

#endif /* CMM920_H_ */
