#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "task.h"

#include "Drivers/PeriphLib/SPI2_TIM8.h"
#include "Drivers/PeriphLib/TIM3.h"
#include "Middle/Stdout/SerialCommand.h"
#include "Middle/Mpu-9250/MPU9250.h"
#include "Middle/AD7176-2/Seeker.hpp"
#include "Middle/MyTasks.h"
#include "Middle/Adis16488/Adis16488.hpp"
#include "Drivers/PeriphLib/ADC3.h"
#include "Drivers/PeriphLib/TIM1.h"
#include "Drivers/PeriphLib/TIM2.h"
#include "Drivers/PeriphLib/TIM3.h"
#include "Drivers/PeriphLib/TIM4.h"
#include "Drivers/PeriphLib/TIM5.h"
#include "Drivers/PeriphLib/USART1.h"
#include "Drivers/PeriphLib/USART2.h"
#include "Drivers/PeriphLib/USART3.h"
#include "Drivers/PeriphLib/I2C2.h"
#include "Drivers/PeriphLib/DAC.h"
#include "Drivers/PeriphLib/USER_FLASH.h"
#include "App/Gains/Gains.h"

#include "stdio.h"
#include "stdlib.h"


/*
 *  stm32F407 Discovery (Xtal = 8MHz)と
 *  stm32F429           (Xtal = 12MHz)での要変更箇所
 *
 *  1.MakefileでのHSE_VALUEの設定,-DSTM32F429のdefine
 *  2.stm32_flash.ldのFLASH,RAMサイズ
 *  3.USER_FLASHの領域設定
 * */

/*
Pin list.
  
	PA0 SW1
	
  HalUsart - XBee
	PD5	USART2_TX
	PD6	USART2_RX
	
  HalSpi1 - Adis16488
	PA5	SPI1_SCK
	PA6 SPI1_MISO
	PA7	SPI1_MOSI
	PE13 DataReady
	PE14 NSS

  HalI2C2 - MPU9250
  	PB10I2C2_SCL
  	PB11 I2C2_SDA
  	XXXX INT

  HalSpi2 - AD7162-2
	PB12 SPI2_NSS
	PB13 SPI2_SCK
	PB14 SPI2_MISO
	PB15 SPI2_MOSI

  ADC - Internal A/D converter
	PC1 ADC3_1
	PC2 ADC3_2
  
  TIM2
  	PA2 TIM2_CH3
  	
  TIM5
    PA3 TIM5_CH4

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
    DMA1Stream5
    DMA1Stream7
    
  HalSpi1
    DMA2Stream2
	DMA2Stream3
	DMA2Stream6
	
  HalSpi2
    DMA1Stream3
    DMA1Stream4
    
  ADC
    DMA2Stream0
    
EXTI 
	EXTI0	Spi1(not used)
	EXTI13	SPI1
	EXTI14	Spi2
*/

void prvTaskA(void *pvParameters){
	int i=0;
	

	
	vTaskDelay(100);
	int data;
	
	
//	FLASHClass::GetInstance()->erase();
	
//	printf("before write\n\r");
//	data = FLASHClass::GetInstance()->read(0);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x1);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x2);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x3);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x4);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x5);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x6);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x7);
//	printf("%x\n\r",data);
//	
//	data = FLASHClass::GetInstance()->read(0);
//	printf("%x\n\r",data);
//	FLASHClass::GetInstance()->write(0,0x5000AAAA);
//	FLASHClass::GetInstance()->write(1,0x5111AAAA);
//	FLASHClass::GetInstance()->write(2,0x5222AAAA);
//	FLASHClass::GetInstance()->write(3,0x5333AAAA);
//	FLASHClass::GetInstance()->flush();
	
//	printf("after write\n\r");
//	data = FLASHClass::GetInstance()->read(0);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x1);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x2);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x3);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x4);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x5);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x6);
//	printf("%x\n\r",data);
//	data = FLASHClass::GetInstance()->read(0x7);
//	printf("%x\n\r",data);
	
	while(1){
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_12);
		vTaskDelay(1);
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)&(~GPIO_Pin_12));
		//printf("taskA\n\r");
		
		vTaskDelay(50);
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

void LEDInit(void) {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef gpiob8;
	GPIO_StructInit(&gpiob8);
	gpiob8.GPIO_Pin = GPIO_Pin_8;
	gpiob8.GPIO_Mode = GPIO_Mode_OUT;
	gpiob8.GPIO_OType = GPIO_OType_PP;
	gpiob8.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&gpiob8);
	
}

int main(void) {
	SystemInit();
	LEDInit();
	
	//GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_12);
	TIM2Class::GetInstance()->timerStart();
	//TIM3Class::GetInstance()->timerStart();
	TIM4Class::GetInstance()->timerStart();
	//TIM1Class::GetInstance()->timerStart();
	
	xTaskCreate(prvTaskA,"TaskA",128,NULL,1,NULL);
	xTaskCreate(prvTaskB,"TaskB",128,NULL,1,NULL);

	GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_13);
	//xTaskCreate(prvTxTask,"u3tx",4096,USART2,1,NULL);
	//xTaskCreate(prvRxTask,"u3rx",4096,USART2,1,NULL);
	//xTaskCreate(&USART1Class::prvTxTask,"u1tx",512,USART1,1,NULL);
	//xTaskCreate(&USART1Class::prvRxTask,"u1rx",1024,USART1,1,NULL);
	//xTaskCreate(&USART2Class::prvTxTask,"u2tx",512,USART2,1,NULL);
	//xTaskCreate(&USART2Class::prvRxTask,"u2rx",1024,USART2,1,NULL);
	xTaskCreate(&USART3Class::prvTxTask,"u3tx",512,USART3,1,NULL);
	xTaskCreate(&USART3Class::prvRxTask,"u3rx",1024,USART3,1,NULL);
	xTaskCreate(&USART2Class::prvTxTask,"u2tx",512,USART2,1,NULL);
	xTaskCreate(&USART2Class::prvRxTask,"u2rx",1024,USART2,1,NULL);
	xTaskCreate(&ADC3Class::prvTask,"ADC",1024,NULL,1,NULL);
	xTaskCreate(&ADIS16488::prvAdis16488Task,"adis",1024,NULL,2,NULL);
	xTaskCreate(prvMpu9250TaskEntry,"mpu",2048,NULL,1,NULL);
//	xTaskCreate(prvI2C2SendTask,"i2c2",512,NULL,1,NULL);
//	xTaskCreate(prvAd7176Task,"ad71",4096,NULL,4,NULL);
//	xTaskCreate(prvSeekerTask,"skr",1024,NULL,2,NULL);
	//xTaskCreate(prvTIM2TestTask,"tim2",128,NULL,1,NULL);
	//xTaskCreate(prvTIM5TestTask,"tim5",128,NULL,1,NULL);
	
	xTaskCreate(&Gains::prvGainsTask,"gains",1024,NULL,1,NULL);
	
	
	vTaskStartScheduler();

	while (1)
	{

	}
}

