/*
 * CanBusMonitor.cpp
 *
 *  Created on: 2015/03/14
 *      Author: sa
 */
#include <pch.h>

#include "CanBusMonitor.h"
#include "Driver/CAN1.h"

CanBusMonitor::CanBusMonitor(){
}

void CanBusMonitor::monitorTask(){
	CAN1Class* can = CAN1Class::GetInstance();
	
	while(1){
		while(can->getFifoCount()!=0){
			CanRxMsg msg = can->receive();
			
			printf("ID:0x%x,",msg.StdId);
			if(msg.RTR){
				printf("RTR,");
			}else{
				printf("DATA,0x");
				for(int i=msg.DLC-1;i>=0;i--){
					printf("%2x",msg.Data[i]);
				}
			}
			printf("\r\n");
			
		}
		vTaskDelay(1);
	}
}

void CanBusMonitor::prvMonitorTaskEntry(void* pvParameters){
	CanBusMonitor::GetInstance()->monitorTask();
}

void CanBusMonitor::initCanBusMonitor(){
	CAN1Class::GetInstance();
	
	xTaskCreate(&CanBusMonitor::prvMonitorTaskEntry,"can",1024,NULL,1,NULL);
}
