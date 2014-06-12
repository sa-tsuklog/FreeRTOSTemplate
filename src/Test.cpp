/*
 * Test.cpp
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "stm32f4xx.h"

#include "Test.h"
#include "GeneralConfig.h"
#include <math.h>
#include <string.h>
//dut
#include "MyLib/Logger/Logger.h"
#include "MyLib/Util/Util.h"

void prvTestTask(void* pvParamters){
	vTaskDelay(MS_INITIAL_DELAY);

	while(1){
		vTaskDelay(2);
	}
}
