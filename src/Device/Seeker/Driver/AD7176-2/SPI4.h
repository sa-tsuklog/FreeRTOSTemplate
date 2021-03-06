#ifndef __PERIPHLIB_SPI4_H__
#define __PERIPHLIB_SPI4_H__

#include "FreeRTOS.h"
#include "semphr.h"

class SPI4Class {
	// Singleton pattern definition
private:
	SPI4Class();
	SPI4Class(const SPI4Class& rhs);
	SPI4Class& operator=(const SPI4Class& rhs);
	virtual ~SPI4Class() {}
public:
	static SPI4Class* GetInstance() {
    	static SPI4Class instance;
    	return &instance;
	}

	// Class definition
private:
	static const int SPI_BUFFERSIZE = 32;
	char m_txBuf[SPI_BUFFERSIZE];
	char m_rxBuf[SPI_BUFFERSIZE];
	SemaphoreHandle_t m_rwSem;
	SemaphoreHandle_t m_dataReadySem;
	void enableEXTI5();
	void disableEXTI5();
public:
	int ReadWrite(unsigned char* outReadData,unsigned char* writeData,int byteRwLength);
	void waitForDataReady();
	void myEXTI5_IRQHandler();
	void myDMA2_Stream1_IRQHandler();
};

#endif /* HALSPI4_H_ */
