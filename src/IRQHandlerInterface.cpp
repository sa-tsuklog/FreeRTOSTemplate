/*
 * IRQHandlerInterface.cpp
 *
 *  Created on: 2014/03/14
 *      Author: lark-mp
 */

#include "IRQHandlerInterface.h"
#include "PeriphLib/SPI2.h"
#include "PeriphLib/I2C2.h"

void myEXTI14_IRQHandler(){
	SPI2Class::GetInstance()->EXTI14_IRQHandler();
}
void myDMA1_Stream3_IRQHandler(){
	SPI2Class::GetInstance()->DMA1_Stream3_IRQHandler();
}

void myI2C2_EV_IRQHandler(){
	I2C2Class::GetInstance()->EV_IRQHandler();
}

void myI2C2_ER_IRQHandler(){
	I2C2Class::GetInstance()->ER_IRQHandler();
}

void myDMA1_Stream2_IRQHandler(){
	I2C2Class::GetInstance()->DMA1_Stream2_IRQHandler();
}

void myDMA1_Stream7_IRQHandler(){
	I2C2Class::GetInstance()->DMA1_Stream7_IRQHandler();
}
