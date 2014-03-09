/*
 * HalSpi2.h
 *
 *  Created on: 2014/03/02
 *      Author: sa
 */

#ifndef HALSPI2_H_
#define HALSPI2_H_

void initSpi2();

void myEXTI14_IRQHandler();
void myDMA1_Stream3_IRQHandler();
int spi2ReadWrite(unsigned char* outReadData,unsigned char* writeData,int byteRwLength);
void waitForDataReady();
void clearSemaphores();
//void myDMA1_Stream4_IRQHandler();
//void mySPI2_IRQHandler();

#endif /* HALSPI2_H_ */
