#ifndef __HALSPI1_H__
#define __HALSPI1_H__

#include "FreeRTOS.h"
#include "semphr.h"

extern xSemaphoreHandle SPI1_TIM1_dataReadySem;

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
	unsigned short m_txBuf[SPI_BUFFERSIZE];
	unsigned short m_rxBuf[SPI_BUFFERSIZE];
	//xSemaphoreHandle m_rwSem;
	
public:
	void setContinuousMode();
	void setSingleTransactionMode();
	int ReadWrite(unsigned char* outReadData,unsigned char* writeData,int byteRwLength);
	void timerStart();
	void waitNewData();
	void TIM1_UP_TIM10_IRQHandler();
	void setTxBuf(int index,unsigned short data);
	unsigned short* getRxBuf();
	unsigned short read16(unsigned char address);
	void write16(unsigned char address,unsigned short data);
	
};

#endif /* __HALSPI1_H__ */
