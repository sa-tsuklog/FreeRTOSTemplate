/*
 * Cmm920.cpp
 *
 *  Created on: 2015/08/10
 *      Author: sa
 */


#include "FreeRTOS.h"
#include "task.h"

#include "Cmm920.h"
#include "USART3_CMM920.h"

#include "GeneralConfig.h"

Cmm920::Cmm920(){
	USART3_CMM920Class::GetInstance();
	txQueue = xQueueCreate(UsartCmm920Config::TX_BUFFERSIZE,sizeof(char));
	if(txQueue == NULL){
		while(1);
	}
	rxQueue = xQueueCreate(UsartCmm920Config::RX_BUFFERSIZE,sizeof(char));
	if(rxQueue == NULL){
		while(1);
	}
	rxMessageQueue = xQueueCreate(UsartCmm920Config::RX_BUFFERSIZE,sizeof(char));
	if(rxMessageQueue == NULL){
		while(1);
	}
	
	messageSendCompleteSem = xSemaphoreCreateBinary();
	
	state = DecodeState::DLE1;
	echo = 0;
}

void Cmm920::Cmm920TxTask(){
	vTaskDelay(MS_INITIAL_DELAY);
	
	requestReset();
	vTaskDelay(10);
	
	requestReset();
	vTaskDelay(10);
	
	requestReset();
	vTaskDelay(10);
	
	setConfigMode();
	vTaskDelay(10);
	
	setRfParameter();
	vTaskDelay(10);
	
	setSendMode();
	vTaskDelay(10);
	
	while(1){
		volatile int messageLength = uxQueueMessagesWaiting(txQueue);
		if(messageLength>0){
			vTaskDelay(5);
		}else{
			vTaskDelay(10);
			continue;
		}
		messageLength = uxQueueMessagesWaiting(txQueue);
		
		if(messageLength > (512-6)){
			messageLength = 512-6;
		}
		
		sendMessage(txQueue,messageLength);
		
	}
}
void Cmm920::setConfigMode(){
	uint8_t sendData[2];
	sendData[0] = 0x02;
	sendData[1] = 0x01;
	sendCommand((short)0x0001, sendData, 2);
}

void Cmm920::setSendMode(){
	uint8_t sendData[2];
	
	sendData[0] = 0x02;
	sendData[1] = 0x02;
	sendCommand((short)0x0001, sendData, 2);
}

void Cmm920::setRfParameter(){
	uint8_t sendData[12];
	
	sendData[0]= 0x02;
	sendData[1]= 0x02;  //50kbps
	sendData[2]= 0x60;  //ch60
	sendData[3]= 0x03;  //TxPower
	sendData[4]= 0x85;  //Tx Carrier SenseLevel
	sendData[5]= 0x95;  //Rx Carrier SenseLevel
	sendData[6]= 0x00;  //Carrier Sense timer
	sendData[7]= 0x82;
	sendData[8]= 0x06;  //Carrier Sense count
	sendData[9]= 0x03;  //RETRY COUNT
	sendData[10]= 0x00;  //RETRY DELAY
	sendData[11]= 0x64;
	
	
	sendCommand((short)0x0012, sendData, 12);
}

void Cmm920::setAddress(uint16_t panId,uint64_t longAddress,uint16_t shortAddress){
	uint8_t sendData[13];
	
	sendData[0] = 0x02;
	sendData[1] = (panId>>8);
	sendData[2] = (panId&0xFF);
	sendData[3] = (longAddress>>56 & 0xFF);
	sendData[4] = (longAddress>>48 & 0xFF);
	sendData[5] = (longAddress>>40 & 0xFF);
	sendData[6] = (longAddress>>32 & 0xFF);
	sendData[7] = (longAddress>>24 & 0xFF);
	sendData[8] = (longAddress>>16 & 0xFF);
	sendData[9] = (longAddress>>8 & 0xFF);
	sendData[10] = (longAddress&0xFF);
	sendData[11] = (shortAddress>>8);
	sendData[12] = (shortAddress&0xFF);
	
	sendCommand((short)0x0011, sendData, 13);
}

void Cmm920::requestReset(){
	uint8_t sendData[1];
	sendData[0] = 0x00;
	sendCommand((short)0x00F2, sendData, 1);
}

void Cmm920::sendCommand(uint16_t cmd,uint8_t* data,int dataLength){
	constructTxHeader(cmd,dataLength);
	constructTxData(data,dataLength);
	USART3_CMM920Class::GetInstance()->Tx(txFrameBuf,11 + dataLength);
}

void Cmm920::constructTxHeader(uint16_t cmd,int dataLength){
	int length = dataLength+5;
	
	txFrameBuf[0] = 0x10;  //DLE
	txFrameBuf[1] = 0x02;  //STX
	txFrameBuf[2] = length>>8;    //LENGTH
	txFrameBuf[3] = length&0xFF;
	txFrameBuf[4] = cmd>>8;  //COMMAND
	txFrameBuf[5] = cmd&0xFF;
	txFrameBuf[6] = 0x00;  //RESULT
	txFrameBuf[7] = 0x00;  //RESULT_DETAIL
	
	
	txFrameBuf[9+dataLength] = 0x10;
	txFrameBuf[10+dataLength] = 0x03;	
}

void Cmm920::constructTxData(uint8_t* data,int dataLength){
	for(int i=0;i<dataLength;i++){
		txFrameBuf[8+i] = data[i];
	}
	uint8_t sum = calcChecksum(dataLength);
	txFrameBuf[8+dataLength] = sum;
}

void Cmm920::sendMessage(xQueueHandle messageQueue,int messageLength){
	constructTxHeader(0x0101,messageLength+3);
	txFrameBuf[8] = 0x00;
	txFrameBuf[9] = (messageLength+4)>>8;
	txFrameBuf[10] = (messageLength+4) & 0xFF;
	
	for(int i=0;i<messageLength;i++){
		uint8_t tmp;
		xQueueReceive(messageQueue,&tmp,0);
		txFrameBuf[11+i] = tmp;
	}
	
	uint8_t sum = calcChecksum(messageLength+3);
	txFrameBuf[messageLength+11] = sum;
	USART3_CMM920Class::GetInstance()->Tx(txFrameBuf,11+3+messageLength);
	
	xSemaphoreTake(messageSendCompleteSem,1000);
}

uint8_t Cmm920::calcChecksum(int dataLength){
	uint8_t sum=0;
	for(int i=2;i<8+dataLength;i++){
		sum += txFrameBuf[i];
	}
	
	return -sum;
}


void Cmm920::Cmm920RxTask(){
	uint8_t buf;
	vTaskDelay(MS_INITIAL_DELAY);
	while(1){
		xQueueReceive(rxQueue,&buf,portMAX_DELAY);
		decodeCommand(buf);
	}
}

void Cmm920::decodeCommand(uint8_t buf){
	if(state == DecodeState::DLE1){
		if(buf == 0x10){
			state = DecodeState::STX;
		}else{
			state = DecodeState::DLE1;
		}
	}else if(state == DecodeState::STX){
		if(buf == 0x02){
			state = DecodeState::DATALEN1;
		}else{
			state = DecodeState::DLE1;
		}
		rxSum = 0;
	}else if(state == DecodeState::DATALEN1){
		state = DecodeState::DATALEN2;
		rxDataLength = (buf<<8)&0xFF00;
		rxSum += buf;
	}else if(state == DecodeState::DATALEN2){
		state = DecodeState::CMD1;
		rxDataLength = rxDataLength | (buf&0xFF);
		rxSum += buf;
	}else if(state == DecodeState::CMD1){
		rxCmd1 = buf;
		rxSum += buf;
		state = DecodeState::CMD2;
	}else if(state == DecodeState::CMD2){
		rxCmd2 = buf;
		rxSum += buf;
		state = DecodeState::RESULT;
	}else if(state == DecodeState::RESULT){
		rxResult = buf;
		rxSum += buf;
		state = DecodeState::RESULT_DETAIL;
	}else if(state == DecodeState::RESULT_DETAIL){
		rxResultDetail = buf;
		rxSum += buf;
		state = DecodeState::DATA;
		rxDataCount = 0;
	}else if(state == DecodeState::DATA){
		if(rxDataCount+5 < rxDataLength){
			rxDataBuffer[rxDataCount] = buf;
			rxSum += buf;
			rxDataCount++;
		}else{	//SUM
			rxSum += buf;
			if(rxSum == 0){
				state = DecodeState::DLE2;
			}else{
				state = DecodeState::DLE1;
			}
		}
	}else if(state == DecodeState::DLE2){
		if(buf == 0x10){
			state = DecodeState::ETX;
		}else{
			state = DecodeState::DLE1;
		}
	}else if(state == DecodeState::ETX){
		if(buf == 0x03 && rxSum == 0){
			decodeData(rxDataBuffer,rxDataCount);
		}
		state = DecodeState::DLE1;
	}
}

void Cmm920::decodeData(uint8_t* dataBuffer, int length){
	if(rxCmd1 == 0x81 && rxCmd2 == 0x01){
		decodeMessage(dataBuffer,length);
	}
}

void Cmm920::decodeMessage(uint8_t* messageBuffer, int length){
	if(rxResult == 0x00 && rxResultDetail == 0x00 && messageBuffer[0] == 0x01){	//receive message
		if(echo){
			for(int i=0;i<length-11;i++){
				if(messageBuffer[5+i] == '\n'){
					putchar('\r');
					putchar('\n');
				}else if(messageBuffer[5+i] == '\r'){
					
				}else if(messageBuffer[5+i] == '\b'){
					putchar('\b');
					putchar(' ');
					putchar('\b');
				}else{
					putchar(messageBuffer[5+i]);
				}
			}
			fflush(stdout);
		}
		
		
		for(int i=0;i<length-11;i++){
			xQueueSend(rxMessageQueue,&messageBuffer[5+i],portMAX_DELAY);
		}
	}else if(rxResult == 0x01 || messageBuffer[0] == 0x00){
		xSemaphoreGive(messageSendCompleteSem);
	}
}

char* Cmm920::readLine(){
	int index = 0;
	char c;
	
	
	while(1){
		xQueueReceive(rxMessageQueue,&c,portMAX_DELAY);
		if(c=='\r'){
		}else if(c=='\n'){
			rxLineBuffer[index] = 0;
			return rxLineBuffer;
		}else if(c == '\b'){
			if(index > 0){
				index--;
			}
		}else{
			rxLineBuffer[index] = c;
			if(index<UsartCmm920Config::LINE_BUFFERSIZE){
				index++;
			}
		}
	}
}

char Cmm920::getChar(){
	char c;
	xQueueReceive(rxMessageQueue,&c,portMAX_DELAY);
	return c;
}

void Cmm920::Cmm920TxTaskEntry(void *pvParameters){
	Cmm920::GetInstance()->Cmm920TxTask();
}

void Cmm920::Cmm920RxTaskEntry(void* pvParameters){
	Cmm920::GetInstance()->Cmm920RxTask();
}

void Cmm920::setEcho(int newState){
	echo = newState;
}

void Cmm920::myUSART3_IRQHandler(){
	USART3_CMM920Class::GetInstance()->myUSART3_IRQHandler();
}

void Cmm920::myDMA1_Stream4IRQHandler(){
	USART3_CMM920Class::GetInstance()->myDMA1_Stream4IRQHandler();
}
