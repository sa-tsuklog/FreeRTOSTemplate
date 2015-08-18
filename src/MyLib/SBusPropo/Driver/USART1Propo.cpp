/*
 * USART1.cpp
 *
 *  Created on: 2014/04/25
 *      Author: sa
 */


#include "USART1Propo.h"
#include "../SBusPropo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "GeneralConfig.h"

USART1ClassPropo::USART1ClassPropo(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	GPIO_InitTypeDef pb6def;

	GPIO_StructInit(&pb6def);
	pb6def.GPIO_Pin = GPIO_Pin_6;
	pb6def.GPIO_Mode = GPIO_Mode_AF;
	pb6def.GPIO_OType = GPIO_OType_PP;
	pb6def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pb6def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&pb6def);

	GPIO_InitTypeDef pb7def;
	GPIO_StructInit(&pb7def);
	pb7def.GPIO_Pin = GPIO_Pin_7;
	pb7def.GPIO_Mode = GPIO_Mode_AF;
	pb7def.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pb7def.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&pb7def);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6,  GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

	USART_InitTypeDef usart1;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	USART_DeInit(USART1);

	USART_StructInit(&usart1);
	usart1.USART_BaudRate = 100000;
	usart1.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart1.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	usart1.USART_Parity = USART_Parity_Even;
	usart1.USART_StopBits = USART_StopBits_2;
	usart1.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART1,&usart1);

	
	USART_Cmd(USART1, ENABLE);

	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
		
	NVIC_InitTypeDef usart1_nvicdef;
	usart1_nvicdef.NVIC_IRQChannel = USART1_IRQn;
	usart1_nvicdef.NVIC_IRQChannelCmd = ENABLE;
	usart1_nvicdef.NVIC_IRQChannelSubPriority = 0xFF;
	usart1_nvicdef.NVIC_IRQChannelPreemptionPriority = 0xFF;
	NVIC_Init(&usart1_nvicdef);
	NVIC_SetPriority(USART1_IRQn,0xFF);

}

void USART1ClassPropo::myUSART1IRQHandler(){
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		portBASE_TYPE switchRequired;
		char c = USART_ReceiveData(USART1);
		xQueueHandle rxQueue = SBusPropo::GetInstance()->getRxQueue();
		xQueueSendFromISR(rxQueue,&c,&switchRequired);
		
	}
}
