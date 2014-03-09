#ifndef __HALSPI1_H__
#define __HALSPI1_H__

#include "stm32f4xx.h"

class SPI1Class {
	// Singleton pattern definition
private:
	SPI1Class();
	SPI1Class(const SPI1Class& rhs);
	SPI1Class& operator=(const SPI1Class& rhs);
	virtual ~SPI1Class() {}
public:
	static SPI1Class* GetInstance() {
    	static SPI1Class instance;
    	return &instance;
	}

	// Class definition
private:
	static const int SPI_BUFFERSIZE = 32;

	char m_txBuf[SPI_BUFFERSIZE];
	char m_rxBuf[SPI_BUFFERSIZE];
public:
	void Send(char* sendData, int size);
	int Recv(char* recvData);
};

#endif /* __HALSPI1_H__ */
