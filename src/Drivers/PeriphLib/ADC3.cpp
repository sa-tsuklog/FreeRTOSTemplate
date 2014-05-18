#include "ADC3.h"
#include "../../GeneralConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Drivers/FreeRTOS_DemoFile/stm32f4xx_it.h"

#include <stdio.h>

ADC3Class::ADC3Class(){
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef 	  gpioa0;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	
	GPIO_StructInit(&gpioa0);
	gpioa0.GPIO_Pin = GPIO_Pin_0;
	gpioa0.GPIO_Mode = GPIO_Mode_AN;
	gpioa0.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpioa0);
	
	ADC_DeInit();
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC3, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC3, ADC_Channel_0, 1, ADC_SampleTime_3Cycles);
	
	ADC_Cmd(ADC3, ENABLE);
}

uint16_t ADC3Class::getNextData(int pinNum)
{
	uint16_t ad =ADC_GetConversionValue(ADC3);
	ADC_ClearFlag(ADC3,ADC_FLAG_AWD|ADC_FLAG_EOC|ADC_FLAG_OVR);
	ADC_SoftwareStartConv(ADC3);
	return ad;
}

void ADC3Class::startFirstConversion(){
	ADC_SoftwareStartConv(ADC3);
}

void ADC3Class::prvTask(void *pvParameters){

	ADC3Class* adc3 = ADC3Class::GetInstance();
	adc3->startFirstConversion();
	volatile uint16_t ad;
	vTaskDelay(MS_INITIAL_DELAY);
	while (1)
	{		
		ad = adc3->getNextData(0) * 3300 / 0xFFF;
		printf("%d\n\r", ad);
		
		
		vTaskDelay(100);
	}
}
