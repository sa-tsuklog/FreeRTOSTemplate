/*
 * Stdout.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */
#include <pch.h>

#include "Stdout.h"
#include "StdoutConfig.h"
#include "Driver/USART3.h"

#include "Common/Console/StreamReader/Wrapper/EscapeSequenceReader.h"
#include "Common/Console/StreamReader/EditableLineReader.h"

Stdout::Stdout(){
	u3txHandle = NULL;
	
	writer = USART3Class::GetInstance();
	StreamReader* hwReader = USART3Class::GetInstance();
	this->escReader = new EscapeSequenceReader(hwReader);
	if(escReader == NULL){
		while(1){}
	}
	reader = new EditableLineReader(escReader,StdoutConfig::LINE_BUF_SIZE,StdoutConfig::COMMAND_LOG_DEPTH,stdout);
	if(reader == NULL){
		while(1){}
	}
}

Stdout::~Stdout(){
	delete escReader;
	delete reader;
}

uint32_t Stdout::getChar(uint32_t msBlockTime){
	return reader->getChar(msBlockTime);
}

uint32_t Stdout::getChar(){
	return reader->getChar();
}

uint8_t* Stdout::readLine(){
	return reader->readLine();
}

xQueueHandle Stdout::getTxQueue(){
	return writer->getTxQueue();
}

void Stdout::putChar(uint8_t c){
	writer->putChar(c);
}

void Stdout::setEcho(int newValue){
	reader->setEcho(newValue);
}

void Stdout::initStdout(){
	//task create.
	Stdout::GetInstance();
	xTaskCreate(&USART3Class::prvTxTask,"u3tx",512,NULL,2,&(Stdout::GetInstance()->u3txHandle));
}
