/*
 * HalI2c2.hpp
 *
 *  Created on: 2014/02/23
 *      Author: sa
 */

#ifndef HALI2C2_HPP_
#define HALI2C2_HPP_

#include "semphr.h"

extern xSemaphoreHandle i2c2Sem;

void initI2c2();
void prvI2C2SendTask(void *pvParameters);


void myI2C2_EV_IRQHandler();
void myI2C2_ER_IRQHandler();
void myDMA1_Stream2_IRQHandler();
void myDMA1_Stream7_IRQHandler();
int i2c2Write(char i2cAddress, char regAddress, char* writeData, int writeLength);
int i2c2Write1(char i2cAddress,char regAddress, char writeData);
int i2c2Read(char i2cAddress,char regAddress, char* readBuf, int readLength);

#endif /* HALI2C2_HPP_ */
