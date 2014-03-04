#include "ADC.h"
#include "FreeRTOS.h"
#include "task.h"

__IO uint16_t ADC3ConvertedValue[2];	//変換結果がDMA転送される配列

void initADC(){
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;

#define ADC3_DR_ADDRESS     ((uint32_t)0x4001224C)	//ADC3の変換結果レジスタのアドレス
  //必要なペリフェラルにクロック供給開始
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

  //DMAの設定
  //送り元のアドレスを固定し、送り先をADC3ConvertedValueにしてアドレスをインクリメント、送るデータ数を2つに
  DMA_InitStructure.DMA_Channel = DMA_Channel_2;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC3_DR_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC3ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 2;	//２チャンネルの変換結果を送るからデータ数は２（チャネルを増やしたらここを変える）
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//送り先アドレスをインクリメント
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

  //GPIOCの指定の入力をアナログに設定
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;	//PC1とPC2を設定
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  ADC_DeInit();
  //ADC3の基本設定
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  //ADC3の変換モード設定。入力2本をスキャン変換モードで逐次変換
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//スキャン変換モードに
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 2;	//入力を２本（チャネルを増やしたらここを変える）
  ADC_Init(ADC3, &ADC_InitStructure);

  //ADC3のアナログ入力を定義する
  //ADC_Channel_11がPA1,ADC_Channel_12がPA2。マニュアル(UM1472)のピン配表を見るとわかる
  //"ADC123_IN11"という表記は、ADC1,ADC2,ADC3で使える入力でchannel_11に割り当てられる、ということ
  //使うチャネルを増やすには、ここに追記する。チャネル指定の次のパラメータは変換シーケンスの順序
  ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 1, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 2, ADC_SampleTime_3Cycles);

  //変換結果がDMA転送されるごとに、ADCは次の変換を開始するように設定
  ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);

  //ADC3のDMAを使えるようにする
  ADC_DMACmd(ADC3, ENABLE);

  //ADC3を使えるようにする
  ADC_Cmd(ADC3, ENABLE);
}

void ADCTask(void *pvParameters){
  //AD変換開始
  ADC_SoftwareStartConv(ADC3);

  uint32_t ADvoltage[2];
  //以下の処理を無限ループで実行するので、デバッガで一時停止しADvoltage配列の内容を読んで確認
  while (1)
  {
	//読み出し結果を電圧に変換して結果を配列に入れる
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
