#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "Stdout/SerialCommand.h"
#include "Mpu-9250/MPU9250.h"
#include "AD7176-2/Ad7176-2Seeker.h"
#include "AD7176-2/Seeker.hpp"
#include "Tasks.h"

#include "stdio.h"

uint32_t Count = 0xFFFFF;

void GPIO_Configuration(void);
void Delay(uint32_t nCount);
void Delay2(uint32_t nCount);


/*
Pin list.
  
	PA0 SW1
	
  HalUsart - XBee
	PA2	USART2_TX
	PA3	USART2_RX
	
  HalSpi1 - Adis16488
	PA4 SPI1_NSS		
	PA5	SPI1_SCK
	PA6 SPI1_MISO
	PA7	SPI1_MOSI

  HalI2C2 - MPU9250
  	PB10I2C2_SCL
  	PB11 I2C2_SDA

  HalSpi2 - AD7162-2
	PB12 SPI2_NSS
	PB13 SPI2_SCK
	PB14 SPI2_MISO
	PB15 SPI2_MOSI

  ADC - Internal A/D converter
	PC1 ADC3_1
	PC2 ADC3_2

  Main
  	PD12 LED
  	PD13 LED
  	PD14 LED
  	PD15 LED

DMA list

  HalUsart
	DMA1Stream5
	DMA1Stream6
	
  HalI2C2
    DMA1Stream2
    DMA1Stream7
    
  HalSpi1
    DMA2Stream2
	DMA2Stream3

  HalSpi2
    DMA1Stream3
    DMA1Stream4
    
  ADC
    DMA2Stream0
    
EXTI 
	EXTI0	Spi1
	EXTI14	Spi2
*/

volatile char stackoverflowTaskname[16];

void prvTaskA(void *pvParameters){
	while(1){
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_12);
		vTaskDelay(100);
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)&(~GPIO_Pin_12));
		vTaskDelay(100);
		//printf("taskA\n\r");
	}
}
void prvTaskB(void *pvParameters){
	while(1){
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_13);
		vTaskDelay(100);
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)&(~GPIO_Pin_13));
		vTaskDelay(100);
		//printf("taskB\n\r");
	}
}

int main(void) {

	SystemInit();
	GPIO_Configuration();

	GPIO_Write(GPIOD, 0);

	xTaskCreate(prvTaskA,(signed portCHAR*)"TaskA",512,NULL,1,NULL);
	xTaskCreate(prvTaskB,(signed portCHAR*)"TaskB",512,NULL,1,NULL);
	xTaskCreate(prvTxTask,(signed portCHAR*)"u3tx",4096,USART2,1,NULL);
	xTaskCreate(prvRxTask,(signed portCHAR*)"u3rx",4096,USART2,1,NULL);
	xTaskCreate(prvAdis16488Task,(signed portCHAR*)"adis",512,NULL,1,NULL);
	xTaskCreate(prvI2C2SendTask,(signed portCHAR*)"i2c2",512,NULL,1,NULL);
	xTaskCreate(ADCTask,(signed portCHAR*)"ADC",512,NULL,2,NULL);
	xTaskCreate(prvAd7176Task,(signed portCHAR*)"ad71",4096,NULL,4,NULL);
	xTaskCreate(prvSeekerTask,(signed portCHAR*)"skr",1024,NULL,2,NULL);

	vTaskStartScheduler();

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
	int i=0;
	while(taskname[i]!=0){
		stackoverflowTaskname[i] = taskname[i];
		i++;
	}
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
