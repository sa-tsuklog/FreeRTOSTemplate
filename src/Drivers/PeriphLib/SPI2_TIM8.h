#ifndef __HALSPI2_H__
#define __HALSPI2_H__

#include "FreeRTOS.h"
#include "semphr.h"

extern xSemaphoreHandle SPI2_TIM8_dataReadySem;

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
	void TIM8_UP_TIM10_IRQHandler();
	void setTxBuf(int index,unsigned short data);
	unsigned short* getRxBuf();
	unsigned short read16(unsigned char address);
	void write16(unsigned char address,unsigned short data);
	
	static void myEXTI6_IRQHandler();
	static void myTIM8_IRQHandler();
	static void myTIM8_CC3_IRQHandler();
};

#endif /* __HALSPI2_H__ */
