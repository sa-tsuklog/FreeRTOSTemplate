#include "ADC1.h"
#include "../../GeneralConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Drivers/FreeRTOS_DemoFile/stm32f4xx_it.h"

#include <stdio.h>

ADC1Class::ADC1Class(){
//	ADC_InitTypeDef       ADC_InitStructure;
//	ADC_CommonInitTypeDef ADC_CommonInitStructure;
//	DMA_InitTypeDef       DMA_InitStructure;
//	GPIO_InitTypeDef 	  gpioa0;
//	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
//
//	DMA_InitStructure.DMA_Channel = DMA_Channel_2;
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1->DR;
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&m_value;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//	DMA_InitStructure.DMA_BufferSize = 2;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
//	DMA_Cmd(DMA2_Stream0, ENABLE);
//	
//	GPIO_StructInit(&gpioa0);
//	gpioa0.GPIO_Pin = GPIO_Pin_0;
//	gpioa0.GPIO_Mode = GPIO_Mode_AN;
//	gpioa0.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOA, &gpioa0);
//	
//	ADC_DeInit();
//	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
//	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
//	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
//	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
//	ADC_CommonInit(&ADC_CommonInitStructure);
//
//	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
//	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
//	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
//	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
//	ADC_InitStructure.ADC_NbrOfConversion = 2;
//	ADC_Init(ADC1, &ADC_InitStructure);
//
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_3Cycles);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 2, ADC_SampleTime_3Cycles);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 3, ADC_SampleTime_3Cycles);
//
//	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
//
//	ADC_DMACmd(ADC1, ENABLE);
//
//	ADC_Cmd(ADC1, ENABLE);
//
//	ADC_SoftwareStartConv(ADC1);
}

uint16_t ADC1Class::GetData(int pinNum)
{
	if(pinNum >= 2){
		// error handling
		printf("Invalid pin number at ADC1\n");
		UsageFault_Handler();
	}
	return m_value[pinNum];
}


void ADC1Class::prvTask(void *pvParameters){

	ADC1Class* adc3 = ADC1Class::GetInstance();
	uint32_t ADvoltage[2];
	vTaskDelay(MS_INITIAL_DELAY);
	while (1)
	{
		ADvoltage[0] = adc3->GetData(0) * 3300 / 0xFFF;
		ADvoltage[1] = adc3->GetData(1) * 3300 / 0xFFF;

		printf("%lx\t%lx\r\n", ADvoltage[0], ADvoltage[1]);
		//printf("%ld\t%ld\r\n", ADvoltage[0], ADvoltage[1]);

		vTaskDelay(100);
	}
}
