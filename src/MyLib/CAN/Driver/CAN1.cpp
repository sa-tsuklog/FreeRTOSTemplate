/*
 * BxCAN.cpp
 *
 *  Created on: 2015/03/13
 *      Author: sa
 */

#include "stm32f4xx.h"
#include <stdio.h>
#include "CAN1.h"

CAN1Class::CAN1Class(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef gpioa11;
	GPIO_StructInit(&gpioa11);
	gpioa11.GPIO_Mode = GPIO_Mode_AF;
	gpioa11.GPIO_OType = GPIO_OType_PP;
	gpioa11.GPIO_Pin = GPIO_Pin_11;
	gpioa11.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpioa11.GPIO_Speed = GPIO_Speed_100MHz;
	
	GPIO_Init(GPIOA,&gpioa11);
	
	GPIO_InitTypeDef gpioa12;
	GPIO_StructInit(&gpioa12);
	gpioa12.GPIO_Mode = GPIO_Mode_AF;
	gpioa12.GPIO_OType = GPIO_OType_PP;
	gpioa12.GPIO_Pin = GPIO_Pin_12;
	gpioa12.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpioa12.GPIO_Speed = GPIO_Speed_100MHz;
	
	GPIO_Init(GPIOA,&gpioa12);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
	
	CAN_InitTypeDef can1;
	CAN_StructInit(&can1);
	can1.CAN_ABOM = ENABLE;
	can1.CAN_AWUM = ENABLE;
	can1.CAN_Mode = CAN_Mode_Normal;
	can1.CAN_NART = DISABLE;
	can1.CAN_RFLM = ENABLE;
	can1.CAN_TTCM = DISABLE;
	can1.CAN_TXFP = DISABLE;
	can1.CAN_Prescaler = 5;
	can1.CAN_SJW = CAN_SJW_1tq;
	can1.CAN_BS1 = CAN_BS1_8tq;
	can1.CAN_BS2 = CAN_BS2_7tq;
	 
	CAN_Init(CAN1,&can1);
	
	CAN_FilterInitTypeDef filter;
	
	filter.CAN_FilterActivation = ENABLE;
	filter.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	filter.CAN_FilterIdHigh = 0xFFFF;
	filter.CAN_FilterIdLow = 0x0000;
	filter.CAN_FilterMaskIdHigh = 0x0000;
	filter.CAN_FilterMaskIdLow = 0x0000;
	filter.CAN_FilterMode = CAN_FilterMode_IdMask;
	filter.CAN_FilterScale = CAN_FilterScale_16bit;
	filter.CAN_FilterNumber = 0;
	
	CAN_FilterInit(&filter);
	
}

void CAN1Class::send(CanTxMsg msg){
	CAN_Transmit(CAN1,&msg);
}

uint32_t CAN1Class::getFifoCount(){
	return CAN_MessagePending(CAN1,CAN_FIFO0);
}

CanRxMsg CAN1Class::receive(){
	CanRxMsg msg;
	CAN_Receive(CAN1,CAN_FIFO0,&msg);
	return msg;
}
