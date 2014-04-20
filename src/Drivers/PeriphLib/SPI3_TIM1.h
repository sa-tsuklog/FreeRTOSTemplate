#ifndef __HALSPI3_H__
#define __HALSPI3_H__

#include "FreeRTOS.h"
#include "semphr.h"

extern xSemaphoreHandle SPI3_TIM1_dataReadySem;

class SPI3Class {
	// Singleton pattern definition
private:
	SPI3Class();
	SPI3Class(const SPI3Class& rhs);
	SPI3Class& operator=(const SPI3Class& rhs);
	virtual ~SPI3Class() {}
public:
	static SPI3Class* GetInstance() {
    	static SPI3Class instance;
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
	void init();
	int ReadWrite(unsigned char* outReadData,unsigned char* writeData,int byteRwLength);
	void timerStart();
	void waitNewData();
	void TIM1_UP_TIM10_IRQHandler();
	void setTxBuf(int index,unsigned short data);
	unsigned short* getRxBuf();
	unsigned short read16(unsigned char address);
	void write16(unsigned char address,unsigned short data);
	
};

#endif /* __HALSPI3_H__ */
