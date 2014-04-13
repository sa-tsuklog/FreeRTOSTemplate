#ifndef __HALSPI1_H__
#define __HALSPI1_H__

#include "FreeRTOS.h"
#include "semphr.h"

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
	//xSemaphoreHandle m_rwSem;
	//xSemaphoreHandle m_dataReadySem;
public:	
	int ReadWrite(unsigned char* outReadData,unsigned char* writeData,int byteRwLength);
	void timerStart();
	void TIM1_UP_TIM10_IRQHandler();
};

void tmpTIM1_UP_TIM10_IRQHandler();

#endif /* __HALSPI1_H__ */
