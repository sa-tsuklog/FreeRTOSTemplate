#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "task.h"

#include "Drivers/PeriphLib/TIM3.h"
#include "Drivers/PeriphLib/SPI1_TIM1.h"
#include "Middle/Stdout/SerialCommand.h"
#include "Middle/Mpu-9250/MPU9250.h"
#include "Middle/AD7176-2/Ad7176-2Seeker.h"
#include "Middle/AD7176-2/Seeker.hpp"
#include "Middle/MyTasks.h"
#include "Drivers/PeriphLib/ADC3.h"

#include "Drivers/PeriphLib/USART2.h"

#include "stdio.h"
#include "stdlib.h"

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
		//printf("taskA\n\r");
	}
}

void LEDInit(void) {
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

	GPIO_Write(GPIOD, 0);
}

int main(void) {

	SystemInit();
	LEDInit();

	GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_12);
	
	TIM3Class::GetInstance()->timerStart();
	
	xTaskCreate(prvTaskA,"TaskA",512,NULL,1,NULL);
	xTaskCreate(prvTaskB,"TaskB",512,NULL,1,NULL);

	GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_13);
	//xTaskCreate(prvTxTask,"u3tx",4096,USART2,1,NULL);
	//xTaskCreate(prvRxTask,"u3rx",4096,USART2,1,NULL);
	xTaskCreate(&USART2Class::prvTxTask,"u3tx",4096,USART2,1,NULL);
	xTaskCreate(&USART2Class::prvRxTask,"u3rx",4096,USART2,1,NULL);
	xTaskCreate(&ADC3Class::prvTask,"ADC",512,NULL,2,NULL);
	xTaskCreate(prvAdis16488Task,"adis",512,NULL,1,NULL);
//	xTaskCreate(prvI2C2SendTask,"i2c2",512,NULL,1,NULL);
//	xTaskCreate(prvAd7176Task,"ad71",4096,NULL,4,NULL);
//	xTaskCreate(prvSeekerTask,"skr",1024,NULL,2,NULL);

	vTaskStartScheduler();

	while (1)
	{

	}
}

