/*
 * IRQHandlerInterface.cpp
 *
 *  Created on: 2014/03/14
 *      Author: lark-mp
 */

#include "IRQHandlerInterface.h"
#include "PeriphLib/SPI2.h"

void myEXTI14_IRQHandler(){
	SPI2Class::GetInstance()->EXTI14_IRQHandler();
}
void myDMA1_Stream3_IRQHandler(){
	SPI2Class::GetInstance()->DMA1_Stream3_IRQHandler();
}

