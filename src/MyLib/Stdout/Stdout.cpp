/*
 * Stdout.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"

#include "Stdout.h"
#include "SerialCommand.h"
#include "Driver/USART3.h"

void Stdout::initStdout(){
	//task create.
	xTaskCreate(&USART3Class::prvTxTask,"u3tx",512,NULL,2,NULL);
	//xTaskCreate(&USART3Class::prvRxTask,"u3rx",1024,USART3,1,NULL);
	xTaskCreate(&SerialCommand::prvSerialCommandTaskEntry,"u3rx",1536,NULL,3,NULL);
}


