/*
 * ImuSpi2.h
 *
 *  Created on: 2016/09/28
 *      Author: sa
 */

#ifndef IMUSPI2_H_
#define IMUSPI2_H_


#include "FreeRTOS.h"
#include "semphr.h"

class ImuSpi2 {
	// Singleton pattern definition
private:
	ImuSpi2();
	ImuSpi2(const ImuSpi2& rhs);
	ImuSpi2& operator=(const ImuSpi2& rhs);
	virtual ~ImuSpi2() {}
public:
	static ImuSpi2* GetInstance() {
    	static ImuSpi2 instance;
    	return &instance;
	}

	// Class definition
private:
	SemaphoreHandle_t rwSem;
	SemaphoreHandle_t dataReadySem;
	void csAssert(uint8_t ch);
	void csDeassert(uint8_t ch);
	
public:
	static const uint8_t CH_ICM20601 = 0;
	static const uint8_t CH_AK8963 = 1;
	static const uint8_t CH_LPS25HB = 2;
	
	
	void init();
	int ReadWrite(uint8_t ch,unsigned char* outReadData,unsigned char* writeData,int byteRwLength);
	void waitNewData();
	
	void myEXTI11_IRQHandler();
};



#endif /* IMUSPI2_H_ */
