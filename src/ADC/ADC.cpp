#include "ADC.h"
#include "FreeRTOS.h"
#include "task.h"

__IO uint16_t ADC3ConvertedValue[2];	//�ϊ����ʂ�DMA�]�������z��

void initADC(){
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;

#define ADC3_DR_ADDRESS     ((uint32_t)0x4001224C)	//ADC3�̕ϊ����ʃ��W�X�^�̃A�h���X
  //�K�v�ȃy���t�F�����ɃN���b�N�����J�n
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

  //DMA�̐ݒ�
  //���茳�̃A�h���X���Œ肵�A������ADC3ConvertedValue�ɂ��ăA�h���X���C���N�������g�A����f�[�^����2��
  DMA_InitStructure.DMA_Channel = DMA_Channel_2;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC3_DR_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC3ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 2;	//�Q�`�����l���̕ϊ����ʂ𑗂邩��f�[�^���͂Q�i�`���l���𑝂₵���炱����ς���j
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//�����A�h���X���C���N�������g
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream0, ENABLE);

  //GPIOC�̎w��̓��͂��A�i���O�ɐݒ�
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;	//PC1��PC2��ݒ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  ADC_DeInit();
  //ADC3�̊�{�ݒ�
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  //ADC3�̕ϊ����[�h�ݒ�B����2�{���X�L�����ϊ����[�h�Œ����ϊ�
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//�X�L�����ϊ����[�h��
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 2;	//���͂��Q�{�i�`���l���𑝂₵���炱����ς���j
  ADC_Init(ADC3, &ADC_InitStructure);

  //ADC3�̃A�i���O���͂��`����
  //ADC_Channel_11��PA1,ADC_Channel_12��PA2�B�}�j���A��(UM1472)�̃s���z�\������Ƃ킩��
  //"ADC123_IN11"�Ƃ����\�L�́AADC1,ADC2,ADC3�Ŏg������͂�channel_11�Ɋ��蓖�Ă���A�Ƃ�������
  //�g���`���l���𑝂₷�ɂ́A�����ɒǋL����B�`���l���w��̎��̃p�����[�^�͕ϊ��V�[�P���X�̏���
  ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 1, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 2, ADC_SampleTime_3Cycles);

  //�ϊ����ʂ�DMA�]������邲�ƂɁAADC�͎��̕ϊ����J�n����悤�ɐݒ�
  ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);

  //ADC3��DMA���g����悤�ɂ���
  ADC_DMACmd(ADC3, ENABLE);

  //ADC3���g����悤�ɂ���
  ADC_Cmd(ADC3, ENABLE);
}

void ADCTask(void *pvParameters){
  //AD�ϊ��J�n
  ADC_SoftwareStartConv(ADC3);

  uint32_t ADvoltage[2];
  //�ȉ��̏����𖳌����[�v�Ŏ��s����̂ŁA�f�o�b�K�ňꎞ��~��ADvoltage�z��̓��e��ǂ�Ŋm�F
  while (1)
  {
	//�ǂݏo�����ʂ�d���ɕϊ����Č��ʂ�z��ɓ����
	ADvoltage[0] = ADC3ConvertedValue[0] *3300/0xFFF;
	ADvoltage[1] = ADC3ConvertedValue[1] *3300/0xFFF;

	GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_14);
	vTaskDelay(ADvoltage[0]);
	GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)&(~GPIO_Pin_14));

	GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)|GPIO_Pin_15);
	vTaskDelay(ADvoltage[1]);
	GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD)&(~GPIO_Pin_15));
  }
}
