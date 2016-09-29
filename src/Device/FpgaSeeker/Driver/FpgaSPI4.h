#ifndef __PERIPHLIB_SPI4_H__
#define __PERIPHLIB_SPI4_H__

#include "FreeRTOS.h"
#include "semphr.h"

class FpgaSPI4Class {
	// Singleton pattern definition
private:
	FpgaSPI4Class();
	FpgaSPI4Class(const FpgaSPI4Class& rhs);
	FpgaSPI4Class& operator=(const FpgaSPI4Class& rhs);
	virtual ~FpgaSPI4Class() {}
public:
	static FpgaSPI4Class* GetInstance() {
    	static FpgaSPI4Class instance;
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
