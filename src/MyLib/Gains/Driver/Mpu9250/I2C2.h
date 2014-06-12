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

extern xSemaphoreHandle m_sem;

class I2C2Class {
	// Singleton pattern definition
private:
	I2C2Class();
	I2C2Class(const I2C2Class& rhs);
	I2C2Class& operator=(const I2C2Class& rhs);
	virtual ~I2C2Class() {}
public:
	static I2C2Class* getInstance() {
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

	char m_txBuf[I2C2_BUFSIZE];
	char m_rxBuf[I2C2_BUFSIZE];
	
	int m_rw;
	int m_address;
	int m_state;

	xSemaphoreHandle m_sem;
	
	void myEV_IRQ_Write();
	void myEV_IRQ_ReadN();
	void myEV_IRQ_Read1();
public:
	int write(char i2cAddress, char regAddress, unsigned char* writeData, int writeLength);
	int write1(char i2cAddress,char regAddress, unsigned char writeData);
	int read(char i2cAddress,char regAddress, unsigned char* readBuf, int readLength);
	
	void myEV_IRQHandler();
	void myER_IRQHandler();
	void myDMA1_Stream2_IRQHandler();
	void myDMA1_Stream7_IRQHandler();
};





#endif /* I2C2_H_ */
