/*
 * I2C2.h
 *
 *  Created on: 2014/03/14
 *      Author: Lark_mp
 */

#ifndef I2C2_H_
#define I2C2_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

class I2C2Class {
	// Singleton pattern definition
private:
	I2C2Class();
	I2C2Class(const I2C2Class& rhs);
	I2C2Class& operator=(const I2C2Class& rhs);
	virtual ~I2C2Class() {}
public:
	static I2C2Class* GetInstance() {
    	static I2C2Class instance;
    	return &instance;
	}

	// Class definition
private:
	static const int RW_WRITE = 0;
	static const int RW_READN = 1;
	static const int RW_READ1 = 2;
	static const int RW_READ_RS = 3;

	static const int STATE_TRANSMIT_DEVADDRESS = 0;
	static const int STATE_RECEIVE_DATA = 1;

	static const int I2C2_BUFSIZE = 16;

	int m_rw;
	int m_address;
	int m_state;

	char m_txBuf[I2C2_BUFSIZE];
	char m_rxBuf[I2C2_BUFSIZE];

	xSemaphoreHandle m_sem;

	void EV_IRQ_Write();
	void EV_IRQ_ReadN();
	void EV_IRQ_Read1();
public:
	int Write(char i2cAddress, char regAddress, char* writeData, int writeLength);
	int Write1(char i2cAddress,char regAddress, char writeData);
	int Read(char i2cAddress,char regAddress, char* readBuf, int readLength);
	void EV_IRQHandler();
	void ER_IRQHandler();
	void DMA1_Stream2_IRQHandler();
	void DMA1_Stream7_IRQHandler();
};

#endif /* I2C2_H_ */
