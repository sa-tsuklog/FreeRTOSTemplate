#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "task.h"

#include "ff.h"

#include "stdio.h"
#include "stdlib.h"

#include "../Libraries/FreeRTOS-Plus/Source/FreeRTOS-Plus-Trace/Include/trcKernelPort.h"

#include "system_stm32f4xx.h"

#include "MyLib/Util/Util.h"
#include "MyLib/Stdout/Stdout.h"
#include "MyLib/Gains/Gains.h"
#include "MyLib/Logger/Logger.h"
#include "Test.h"
#include "MyLib/Gains/Driver/Mpu9250/MPU9250.h"
#include "MyLib/CmdServo/CmdServo.h"
#include "App/TankControl/TankControl.h"
#include "MyLib/Seeker/Driver/AD7176-2/Ad7176-2Seeker.h"
#include "MyLib/SignalGenerator/Driver/DAC_TIM8.h"
#include "MyLib/CAN/Driver/CAN1.h"
#include "MyLib/CAN/CanBusMonitor.h"
/*
 *  stm32F407 Discovery (Xtal = 8MHz)と
 *  stm32F429           (Xtal = 12MHz)での要変更箇所
 *
 *  1.MakefileでのHSE_VALUEの設定,-DSTM32F429のdefine
 *  2.stm32_flash.ldのFLASH,RAMサイズ
 *  3.USER_FLASHの領域設定
 * */

//TODO: 動作中にSDカードの抜き差しをした場合、最初にfopenした時の状態のつもりで動作している模様。
//      抜いてから再度fopenしても、エラーメッセージが出ない。

extern unsigned int idle_count;
void prvTaskA(void *pvParameters){
	vTaskDelay(100);
	unsigned int previousIdleCount = 0;
	while(1){
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_12);
		GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)&(~GPIO_Pin_12));
		//Util::GetInstance()->myFprintf(0,stdout,"idle:%d\r\n",idle_count-previousIdleCount);
		previousIdleCount = idle_count;
		
		//CAN1Class::GetInstance()->send();
		
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
	
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
}

int main(void) {
	SystemInit();
	LEDInit();
	
	//CAN1Class::GetInstance();
	
	Servo::GetInstance()->start();
	Util::initUtil();
	Stdout::initStdout();
	Gains::initGains();
	//Logger::initLogger();
	//CmdServo::initCmdServo();
	//CanBusMonitor::initCanBusMonitor();
	
	xTaskCreate(prvTestTask,"test",2048,NULL,2,NULL);
	//TankControl::initTankControl();
	xTaskCreate(prvTaskA,"taskA",2048,NULL,2,NULL);
	//xTaskCreate(&Ad7176_2Seeker::prvAd7176_2TaskEntry,"seeker",2048,NULL,2,NULL);
	
	vTaskStartScheduler();
	
	while (1)
	{
	}
}
