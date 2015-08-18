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
#include "MyLib/Gains/Driver/Mpu9250/I2C2.h"
#include "MyLib/Gains/Driver/Adis16488/SPI2_TIM8.h"
#include "MyLib/Seeker/Driver/AD7176-2/SPI4.h"
#include "MyLib/Stdout/Stdout.h"
#include "MyLib/SBusPropo/Driver/USART1Propo.h"

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
		SPI2Class::myEXTI6_IRQHandler();
	}else if(EXTI_GetITStatus(EXTI_Line5)!=RESET){
		EXTI_ClearITPendingBit(EXTI_Line5);
		SPI4Class::GetInstance()->myEXTI5_IRQHandler();
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
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET){
		EXTI_ClearITPendingBit(EXTI_Line12);
		I2C2Class::getInstance()->myEXTI12IRQHandler();
	}
	//SPI2Class::GetInstance()->EXTI14_IRQHandler();
}

void I2C2_EV_IRQHandler()
{
	I2C2Class::getInstance()->myEV_IRQHandler();
}

void I2C2_ER_IRQHandler()
{
	I2C2Class::getInstance()->myER_IRQHandler();
}

void USART1_IRQHandler()
{
	USART1ClassPropo::GetInstance()->myUSART1IRQHandler();
}

void USART3_IRQHandler(){
	Stdout::GetInstance()->myUSART3IRQHandler();
}

void DMA1_Stream1_IRQHandler()
{
}

void DMA1_Stream2_IRQHandler()
{
	I2C2Class::getInstance()->myDMA1_Stream2_IRQHandler();
}
void DMA1_Stream3_IRQHandler()
{
	//SPI2Class::GetInstance()->DMA1_Stream3_IRQHandler();
}
void DMA1_Stream4_IRQHandler()
{
	Stdout::GetInstance()->myDma1_Stream4IRQHandler();
}
void DMA1_Stream7_IRQHandler()
{
	I2C2Class::getInstance()->myDMA1_Stream7_IRQHandler();
}

//void DMA2_Stream6_IRQHandler()	//defined in Libraries/ff/sdio_stm32f4.h
//{
//	
//}
void DMA2_Stream1_IRQHandler()
{
	SPI4Class::GetInstance()->myDMA2_Stream1_IRQHandler();
}
void DMA2_Stream7_IRQHandler()
{
}
void TIM8_CC3_IRQHandler(){
	SPI2Class::myTIM8_CC3_IRQHandler();
}

void TIM8_UP_TIM13_IRQHandler(){
	if(TIM_GetITStatus(TIM8,TIM_IT_Update)!=RESET){
		SPI2Class::myTIM8_IRQHandler();
	}
}

void TIM2_IRQHandler(){
}

void TIM5_IRQHandler(){
	if(TIM_GetITStatus(TIM5,TIM_IT_CC2)!=RESET){
		TIM_ClearITPendingBit(TIM5,TIM_IT_CC2);
	}
}

void TIM6_DAC_IRQHandler(){
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
}

#ifdef __cplusplus
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
