/*
 * Adis16488.cpp
 *
 *  Created on: 2014/02/19
 *      Author: sa
 */

#include "stdlib.h"
#include "stdio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "Middle/Adis16488/Adis16488.hpp"
#include "Middle/Adis16488/Adis16488RegisterDefs.h"

void write16(unsigned char address,unsigned short writedata){
	
}
unsigned short read16(unsigned char address){
	
}


void ADiadis16488init(){
	unsigned short tmp;
	tmp = read16(PROD_ID);
	printf("%d\n\r",tmp);
	
	
	
	
	
	
}
