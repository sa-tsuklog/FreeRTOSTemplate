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
#include "Middle/Stdout/myFileHandle.h"
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
#include "ff.h"

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


extern unsigned int idle_count;
void prvTaskA(void *pvParameters){
	vTaskDelay(100);
	unsigned int previousIdleCount = 0;
	while(1){
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_12);
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)&(~GPIO_Pin_12));
		printf("idle:%d\r\n",idle_count-previousIdleCount);
		previousIdleCount = idle_count;
		vTaskDelay(100);
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
	//TIM4Class::GetInstance()->timerStart();
	//TIM1Class::GetInstance()->timerStart();
	
	//xTaskCreate(prvTaskA,"TaskA",512,NULL,4,NULL);

	xTaskCreate(&USART1Class::prvTxTask,"u1tx",512,USART1,2,NULL);
	xTaskCreate(&USART1Class::prvRxTask,"u1rx",1024,USART1,2,NULL);
	xTaskCreate(&USART2Class::prvTxTask,"u2tx",512,USART2,2,NULL);
	xTaskCreate(&USART2Class::prvRxTask,"u2rx",1024,USART2,2,NULL);
	xTaskCreate(&USART3Class::prvTxTask,"u3tx",512,USART3,2,NULL);
	xTaskCreate(&USART3Class::prvRxTask,"u3rx",1024,USART3,1,NULL);
	
	xTaskCreate(&ADC3Class::prvTask,"ADC",1024,NULL,2,NULL);
	xTaskCreate(&ADIS16488::prvAdis16488Task,"adis",1024,NULL,4,NULL);
	//xTaskCreate(prvMpu9250TaskEntry,"mpu",2048,NULL,1,NULL);
	//xTaskCreate(prvI2C2SendTask,"i2c2",512,NULL,1,NULL);
	//xTaskCreate(prvAd7176Task,"ad71",4096,NULL,4,NULL);
	//xTaskCreate(prvSeekerTask,"skr",1024,NULL,2,NULL);
	//xTaskCreate(prvTIM2TestTask,"tim2",128,NULL,1,NULL);
	//xTaskCreate(prvTIM5TestTask,"tim5",128,NULL,1,NULL);
	
	xTaskCreate(&Gains::prvGainsTask,"gains",1024,NULL,2,NULL);
	
	
	vTaskStartScheduler();

	while (1)
	{

	}
}

