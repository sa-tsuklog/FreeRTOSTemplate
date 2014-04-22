/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Template/stm32f4xx_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    30-September-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stm32f4xx_it.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "Drivers/PeriphLib/SPI4.h"
#include "Drivers/PeriphLib/I2C2.h"
#include "Drivers/PeriphLib/SPI2_TIM8.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
	vPortSVCHandler();
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
	xPortPendSVHandler();
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	xPortSysTickHandler();
}

/**
  * @brief  This function handles EXTI 3 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line6)!=RESET){
		EXTI_ClearITPendingBit(EXTI_Line6);
		//SPI2Class::GetInstance()->timerStart();
	}
}

/**
  * @brief  This function handles EXTI 15-10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(){
	if(EXTI_GetITStatus(EXTI_Line0)!=RESET){
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
void EXTI15_10_IRQHandler()
{
	
	//SPI2Class::GetInstance()->EXTI14_IRQHandler();
}

void DMA1_Stream3_IRQHandler()
{
	//SPI2Class::GetInstance()->DMA1_Stream3_IRQHandler();
}

void I2C2_EV_IRQHandler()
{
	//I2C2Class::GetInstance()->EV_IRQHandler();
}

void I2C2_ER_IRQHandler()
{
	//I2C2Class::GetInstance()->ER_IRQHandler();
}

void DMA1_Stream2_IRQHandler()
{
	//I2C2Class::GetInstance()->DMA1_Stream2_IRQHandler();
}

void DMA1_Stream7_IRQHandler()
{
	//I2C2Class::GetInstance()->DMA1_Stream7_IRQHandler();
}
void DMA2_Stream6_IRQHandler()
{
	printf("test\n\r");
}
void TIM8_CC3_IRQHandler(){
	if(TIM_GetITStatus(TIM8,TIM_IT_CC3)!=RESET){
		TIM_ClearITPendingBit(TIM8,TIM_IT_CC3);
	}
}


void TIM8_UP_TIM13_IRQHandler(){
	portBASE_TYPE xSwitchRequired;
	if(TIM_GetITStatus(TIM8,TIM_IT_Update)!=RESET){
		TIM_ClearITPendingBit(TIM8,TIM_IT_Update);
		xSemaphoreGiveFromISR(SPI2_TIM8_dataReadySem,&xSwitchRequired);
	}
	portEND_SWITCHING_ISR(xSwitchRequired );
}

void TIM2_IRQHandler(){
	if(TIM_GetITStatus(TIM2,TIM_IT_CC1)!=RESET){
		TIM_ClearITPendingBit(TIM2,TIM_IT_CC1);
	}
	
}

void TIM5_IRQHandler(){
	if(TIM_GetITStatus(TIM5,TIM_IT_CC2)!=RESET){
		TIM_ClearITPendingBit(TIM5,TIM_IT_CC2);
	}
	
}

#ifdef __cplusplus
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
