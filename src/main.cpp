#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#include "Stdout/HalUsart.h"
#include "Adis16488/HalSpi.hpp"
#include "Mpu-9250/HalI2c2.h"
#include "Servo/HalTim.h"

#include "stdio.h"
#include <vector>

using namespace std;

uint32_t Count = 0xFFFFF;

void GPIO_Configuration(void);
void Delay(uint32_t nCount);
void Delay2(uint32_t nCount);


void prvTaskA(void *pvParameters){
	while(1){

		GPIO_Write(GPIOD, GPIO_Pin_12);
		vTaskDelay(100);
		GPIO_Write(GPIOD, 0);
		vTaskDelay(2000);
		printf("taskA\n\r");
	}
}
void prvTaskB(void *pvParameters){
	while(1){
		GPIO_Write(GPIOD, GPIO_Pin_13);
		vTaskDelay(100);
		GPIO_Write(GPIOD, 0);
		vTaskDelay(100);
		printf("taskB\n\r");
	}
}


int main(void) {
	SystemInit();
	GPIO_Configuration();
	initUart(USART2);
	initSpi();
	initI2c2();
	initTim();

	xTaskCreate(prvTaskA,(signed portCHAR*)"TaskA",512,NULL,2,NULL);
	xTaskCreate(prvTaskB,(signed portCHAR*)"TaskA",512,NULL,1,NULL);
	xTaskCreate(prvTxTask,(signed portCHAR*)"u3tx",192,USART2,4,NULL);
	xTaskCreate(prvRxTask,(signed portCHAR*)"u3rx",1024,USART2,1,NULL);
	xTaskCreate(prvAdis16488Task,(signed portCHAR*)"adis",512,NULL,1,NULL);
	xTaskCreate(prvI2C2SendTask,(signed portCHAR*)"i2c2",512,NULL,2,NULL);
	
	vTaskStartScheduler();

	/* Blink LED */
	while (1)
	{

	}
}

void vApplicationIdleHook(void){
	static int idle_count=0;
	idle_count++;
}

void vApplicationMallocFailedHook(void){
	while(1);
}
void vApplicationStackOverflowHook(void* ptr, signed char* taskname){
	while(1);
}
void vApplicationTickHook(){

}

void GPIO_Configuration(void) {
	//Supply AHB1 Clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure OB_LED: output push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
			| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void Delay(uint32_t nCount) {
	volatile uint32_t cnt = nCount;
	for (; cnt != 0; cnt--)
		;
}
void Delay2(uint32_t nCount){
	for(;nCount !=0 ; nCount--){

	}
}
