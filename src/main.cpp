#include <pch.h>
#include "stm32f4xx_rcc.h"

#include "ff.h"

#include "../Libraries/FreeRTOS-Plus/Source/FreeRTOS-Plus-Trace/Include/trcKernelPort.h"

#include "system_stm32f4xx.h"

#include "Test.h"

#include "Device/Util/Util.h"
#include "Device/Stdout/Stdout.h"
#include "Device/Gains/Gains.h"
#include "Device/Logger/Logger.h"
#include "Device/Gains/Driver/Mpu9250/MPU9250.h"
#include "Device/CmdServo/CmdServo.h"
#include "Device/Seeker/Seeker.h"
#include "Device/SignalGenerator/Driver/DAC_TIM8.h"
#include "Device/CAN/Driver/CAN1.h"
#include "Device/CAN/CanBusMonitor.h"
#include "Device/SBusPropo/SBusPropo.h"
#include "Device/FpgaSeeker/FpgaSeeker.h"
#include "App/GliderControl/GliderControl.h"
#include "App/MissileControl/MissileControl.h"
#include "App/SerialCommand/SerialCommand.h"
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
	int i=0;
	while(1){
		
		//Util::GetInstance()->myFprintf(0,stdout,"idle:%d\r\n",idle_count-previousIdleCount);
		//fflush(stdout);
		//CAN1Class::GetInstance()->send();
		printf("%s\r\n",TEST_MESSAGE);
		vTaskDelay(100);
		i++;
	}
	
}

void LEDInit(void) {
	
}

int main(void) {
	SystemInit();
	LEDInit();
	
	//CAN1Class::GetInstance();
	
	Servo::GetInstance()->start();
	Util::initUtil();
	Stdout::initStdout();
	SerialCommand::initSerialCommand();
	Gains::initGains();
	Logger::initLogger();
	//CmdServo::initCmdServo();
	//CanBusMonitor::initCanBusMonitor();
	//Seeker::initSeeker();
	FpgaSeeker::initFpgaSeeker();
	//SBusPropo::initSBusPropo();
	//xTaskCreate(prvTestTask,"test",1024,NULL,2,NULL);
	//GliderControl::initGliderControl();
	MissileControl::initMissileControl();
	
	vTaskStartScheduler();
	
	
	while (1)
	{
	}
}
