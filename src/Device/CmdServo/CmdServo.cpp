/*
 * CmdServo.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */
#include <pch.h>

#include "CmdServo.h"

CmdServo::CmdServo(){
	usart = USART1Class::GetInstance();
}

void CmdServo::sendShortPacket(unsigned char id,unsigned char flag,unsigned char address,unsigned char length,unsigned char* data){
	char checksum = id ^ flag ^ address ^ length ^ 0x01;
	
	usart->uputchar(0xFA);	//header
	usart->uputchar(0xAF);	//header
	usart->uputchar(id);
	usart->uputchar(flag);
	usart->uputchar(address);
	usart->uputchar(length);
	usart->uputchar(0x01);	//cnt
	
	for(int i=0;i<length;i++){
		usart->uputchar(data[i]);
		checksum ^= data[i];
	}
	
	usart->uputchar(checksum);
}

void CmdServo::setMargin(unsigned char id,unsigned char mdeg_x100Margin,int isCw){
	unsigned char address;
	if(isCw){
		address = 0x18;
	}else{
		address = 0x19;
	}
	sendShortPacket(id,0x00,address,0x01,&mdeg_x100Margin);
}
void CmdServo::setSlope(unsigned char id,unsigned char degSlope,int isCw){
	unsigned char address;
	if(isCw){
		address = 0x1A;
	}else{
		address = 0x1B;
	}
	sendShortPacket(id,0x00,address,0x01,&degSlope);
}
void CmdServo::setPunch(unsigned char id,unsigned short mpercent_x10Punch){
	sendShortPacket(id,0x00,0x1C,0x02,(unsigned char*)&mpercent_x10Punch);
}

void CmdServo::on(unsigned char id){
	unsigned char data = 0x01;
	sendShortPacket(id,(unsigned char)0x00,(unsigned char)0x24,(unsigned char)0x01,&data);
}
void CmdServo::setPos(unsigned char id,float degPos){
	short data = (short)(degPos*10);
	if(data > 1500){
		data = 1500;
	}else if(data < -1500){
		data = -1500;
	}
	
	sendShortPacket(id,0x00,0x1E,0x02,(unsigned char*)&data);
}

void CmdServo::setServoId(unsigned char oldId, unsigned char newId){
	sendShortPacket(oldId,0x00,0x04,0x01,&newId);
	sendShortPacket(newId,0x40,0xFF,0x00,NULL);
}

void CmdServo::setAngleLimit(unsigned char id,float degLimit){
	short data = (short)(degLimit*10);
	unsigned char address;
	if(degLimit > 1500){
		data = 1500;
	}else if(data < -1500){
		data = -1500;
	}
	if(data < 0){
		address = 0x0A;
	}else{
		address = 0x08;
	}
	
	sendShortPacket(id,0x00,address,0x02,(unsigned char*)&data);
}

void CmdServo::setMoveTime(unsigned char id,unsigned int ms_x10MoveTime){
	sendShortPacket(id,0x00,0x1E,0x04,(unsigned char*)&ms_x10MoveTime);
}

void CmdServo::setTorqueLimit(unsigned char id,unsigned char percentTorqueLimit){
	sendShortPacket(id,0x00,0x23,0x01,&percentTorqueLimit);
}

void CmdServo::flashFinalize(unsigned char id){
	unsigned char checksum = id ^ 0x40 ^ 0xFF ^ 0x00 ^ 0x00;

	usart->uputchar(0xFA);
	usart->uputchar(0xAF);
	usart->uputchar(id);
	usart->uputchar(0x40);
	usart->uputchar(0xFF);
	usart->uputchar(0x00);
	usart->uputchar(0x00);
	usart->uputchar(checksum);
}

void CmdServo::initCmdServo(){
	xTaskCreate(USART1Class::prvTxTask,"u1tx",512,NULL,2,&(CmdServo::GetInstance()->u1txHandle));
}
