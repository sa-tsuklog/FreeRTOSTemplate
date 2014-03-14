/*
 * IRQHandlerInterface.h
 *
 *  Created on: 2014/03/14
 *      Author: lark-mp
 */

#ifndef __IRQ_HANDLER_INTERFACE_H__
#define __IRQ_HANDLER_INTERFACE_H__

extern "C"{

void myEXTI14_IRQHandler();
void myDMA1_Stream3_IRQHandler();

void myI2C2_EV_IRQHandler();
void myI2C2_ER_IRQHandler();
void myDMA1_Stream2_IRQHandler();
void myDMA1_Stream7_IRQHandler();

}

#endif /* IRQHANDLERINTERFACE_H_ */
