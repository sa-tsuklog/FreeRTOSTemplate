/*
 * Stdout.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */
#include <pch.h>

#include "Stdout.h"
#include "SerialCommand.h"
#include "Driver/USART3.h"
#include "DriverCmm920/Cmm920.h"

Stdout::Stdout(){
	////for normal usart.
	//io = USART3Class::GetInstance();
	
	////for usart via cmm920.
	io = Cmm920::GetInstance();
}

void Stdout::initStdout(){
	//task create.
	
	////for normal usart.
	//xTaskCreate(&USART3Class::prvTxTask,"u3tx",512,NULL,2,&(Stdout::GetInstance()->u3txHandle));
	
	////for usart via cmm920.
	xTaskCreate(&Cmm920::Cmm920TxTaskEntry,"u3tx",512,NULL,2,&(Stdout::GetInstance()->u3txHandle));
	xTaskCreate(&Cmm920::Cmm920RxTaskEntry,"u3rx",512,NULL,3,&(Stdout::GetInstance()->u3rxHandle));
	
	////for both normal and cmm920 uart.
	xTaskCreate(&SerialCommand::prvSerialCommandTaskEntry,"cmd",1024,NULL,2,&(Stdout::GetInstance()->cmdHandle));
}

char* Stdout::readLine(){
	return io->readLine();
}

char Stdout::getChar(){
	return io->getChar();
}

xQueueHandle Stdout::getTxQueue(){
	return io->getTxQueue();
}

void Stdout::myUSART3IRQHandler(){
	io->myUSART3_IRQHandler();
}

void Stdout::myDma1_Stream4IRQHandler(){
	io->myDMA1_Stream4IRQHandler();
}

void Stdout::setEcho(int newValue){
	io->setEcho(newValue);
}
