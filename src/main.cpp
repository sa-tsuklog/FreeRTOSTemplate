#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "task.h"

#include "ff.h"

#include "stdio.h"
#include "stdlib.h"

#include "Common/FreeRTOS_DemoFile/system_stm32f4xx.h"

#include "MyLib/Util/Util.h"
#include "MyLib/Stdout/Stdout.h"
#include "MyLib/Gains/Gains.h"
#include "MyLib/Logger/Logger.h"
#include "Test.h"
#include "MyLib/Gains/Driver/Mpu9250/MPU9250.h"
#include "MyLib/CmdServo/CmdServo.h"

/*
 *  stm32F407 Discovery (Xtal = 8MHz)��
 *  stm32F429           (Xtal = 12MHz)�ł̗v�ύX�ӏ�
 *
 *  1.Makefile�ł�HSE_VALUE�̐ݒ�,-DSTM32F429��define
 *  2.stm32_flash.ld��FLASH,RAM�T�C�Y
 *  3.USER_FLASH�̗̈�ݒ�
 * */

//TODO: ���쒆��SD�J�[�h�̔��������������ꍇ�A�ŏ���fopen�������̏�Ԃ̂���œ��삵�Ă���͗l�B
//      �����Ă���ēxfopen���Ă��A�G���[���b�Z�[�W���o�Ȃ��B

extern unsigned int idle_count;
void prvTaskA(void *pvParameters){
	vTaskDelay(100);
	unsigned int previousIdleCount = 0;
	while(1){
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_12);
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)&(~GPIO_Pin_12));
		Util::GetInstance()->myFprintf(0,stdout,"idle:%d\r\n",idle_count-previousIdleCount);
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
	
	Util::initUtil();
	Stdout::initStdout();
	Gains::initGains();
	Logger::initLogger();
	CmdServo::initCmdServo();
	
	xTaskCreate(prvTestTask,"test",2048,NULL,2,NULL);
	
	
	vTaskStartScheduler();

	while (1)
	{

	}
}
