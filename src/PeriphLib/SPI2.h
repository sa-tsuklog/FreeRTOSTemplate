#ifndef __PERIPHLIB_SPI2_H__
#define __PERIPHLIB_SPI2_H__

#include "FreeRTOS.h"
#include "semphr.h"

class SPI2Class {
	// Singleton pattern definition
private:
	SPI2Class();
	SPI2Class(const SPI2Class& rhs);
	SPI2Class& operator=(const SPI2Class& rhs);
	virtual ~SPI2Class() {}
public:
	static SPI2Class* GetInstance() {
    	static SPI2Class instance;
    	return &instance;
	}

	// Class definition
private:
	static const int SPI_BUFFERSIZE = 32;
	char m_txBuf[SPI_BUFFERSIZE];
	char m_rxBuf[SPI_BUFFERSIZE];
	xSemaphoreHandle m_rwSem;
	xSemaphoreHandle m_dataReadySem;
public:
	int ReadWrite(unsigned char* outReadData,unsigned char* writeData,int byteRwLength);
	void WaitForDataReady();
	void EXTI14_IRQHandler();
	void DMA1_Stream3_IRQHandler();
};

#endif /* HALSPI2_H_ */
