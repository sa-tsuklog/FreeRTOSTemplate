#ifndef __ADC_H_
#define __ADC_H_

#include "stm32f4xx.h"

void initADC();
void ADCTask(void *pvParameters);

#endif
