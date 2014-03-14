/**
  ******************************************************************************
  * @file    RCC/RCC_Example/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "Drivers/IRQHandlerInterface.h"

#include "stdio.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup RCC_Example
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
  /* This interrupt is generated when HSE clock fails */

  if (RCC_GetITStatus(RCC_IT_CSS) != RESET)
  {
    /* At this stage: HSE, PLL are disabled (but no change on PLL config) and HSI
       is selected as system clock source */

    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    /* Enable HSE Ready and PLL Ready interrupts */
    RCC_ITConfig(RCC_IT_HSERDY | RCC_IT_PLLRDY, ENABLE);

    /* Clear Clock Security System interrupt pending bit */
    RCC_ClearITPendingBit(RCC_IT_CSS);

    /* Once HSE clock recover, the HSERDY interrupt is generated and in the RCC ISR
       routine the system clock will be reconfigured to its previous state (before
       HSE clock failure) */
  }
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
//  while (1)
//  {
//  }
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
  * moved to Libraries/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
  */
void vPortSVCHandler();
void xPortPendSVHandler();
void xPortSysTickHandler();

/*
void SVC_Handler(void)
{
	vPortSVCHandler();
}
*/
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
  * moved to Libraries/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
  */
/*
void PendSV_Handler(void)
{
	xPortPendSVHandler();
}
*/

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  * moved to Libraries/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
  */
/*
void SysTick_Handler(void)
{
	xPortSysTickHandler();
}
*/

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @brief  This function handles RCC interrupt request. 
  * @param  None
  * @retval None
  */
void RCC_IRQHandler(void)
{
  if(RCC_GetITStatus(RCC_IT_HSERDY) != RESET)
  { 
    /* Clear HSERDY interrupt pending bit */
    RCC_ClearITPendingBit(RCC_IT_HSERDY);

    /* Check if the HSE clock is still available */
    if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET)
    { 
      /* Enable PLL: once the PLL is ready the PLLRDY interrupt is generated */ 
      RCC_PLLCmd(ENABLE);     
    }
  }

  if(RCC_GetITStatus(RCC_IT_PLLRDY) != RESET)
  { 
    /* Clear PLLRDY interrupt pending bit */
    RCC_ClearITPendingBit(RCC_IT_PLLRDY);

    /* Check if the PLL is still locked */
    if (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != RESET)
    { 
      /* Select PLL as system clock source */
      RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    }
  }
}

void WWDG_IRQHandler(){
	while(1);

}                   /* Window WatchDog              */
void PVD_IRQHandler(){}                    /* PVD through EXTI Line detection */
void TAMP_STAMP_IRQHandler(){}             /* Tamper and TimeStamps through the EXTI line */
void RTC_WKUP_IRQHandler(){}               /* RTC Wakeup through the EXTI line */
void FLASH_IRQHandler(){}                  /* FLASH                        */
void EXTI0_IRQHandler(){
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
		EXTI_ClearITPendingBit(EXTI_Line0);
		printf("irq0\n\r");
	}
}                  /* EXTI Line0                   */
void EXTI1_IRQHandler(){}                  /* EXTI Line1                   */
void EXTI2_IRQHandler(){}                  /* EXTI Line2                   */
void EXTI3_IRQHandler(){}                  /* EXTI Line3                   */
void EXTI4_IRQHandler(){}                  /* EXTI Line4                   */
void DMA1_Stream0_IRQHandler(){}           /* DMA1 Stream 0                */
void DMA1_Stream1_IRQHandler(){}           /* DMA1 Stream 1                */
void DMA1_Stream2_IRQHandler(){
	myDMA1_Stream2_IRQHandler();
}           /* DMA1 Stream 2                */
void DMA1_Stream3_IRQHandler(){
	myDMA1_Stream3_IRQHandler();
}           /* DMA1 Stream 3                */
void DMA1_Stream4_IRQHandler(){}           /* DMA1 Stream 4                */
void DMA1_Stream5_IRQHandler(){}           /* DMA1 Stream 5                */
void DMA1_Stream6_IRQHandler(){}           /* DMA1 Stream 6                */
void DMA1_Stream7_IRQHandler(){
	myDMA1_Stream7_IRQHandler();
}           /* DMA1 Stream 7                */
void ADC_IRQHandler(){}                    /* ADC1, ADC2 and ADC3s         */
void CAN1_TX_IRQHandler(){}                /* CAN1 TX                      */
void CAN1_RX0_IRQHandler(){}               /* CAN1 RX0                     */
void CAN1_RX1_IRQHandler(){}               /* CAN1 RX1                     */
void CAN1_SCE_IRQHandler(){}               /* CAN1 SCE                     */
void EXTI9_5_IRQHandler(){}                /* External Line[9:5]s          */
void TIM1_BRK_TIM9_IRQHandler(){}          /* TIM1 Break and TIM9          */
void TIM1_UP_TIM10_IRQHandler(){}          /* TIM1 Update and TIM10        */
void TIM1_TRG_COM_TIM11_IRQHandler(){}     /* TIM1 Trigger and Commutation and TIM11 */
void TIM1_CC_IRQHandler(){}                /* TIM1 Capture Compare         */
void TIM2_IRQHandler(){}                   /* TIM2                         */
void TIM3_IRQHandler(){}                   /* TIM3                         */
void TIM4_IRQHandler(){}                   /* TIM4                         */
void I2C1_EV_IRQHandler(){}                /* I2C1 Event                   */
void I2C1_ER_IRQHandler(){}                /* I2C1 Error                   */
void I2C2_EV_IRQHandler(){
	myI2C2_EV_IRQHandler();
}                /* I2C2 Event                   */
void I2C2_ER_IRQHandler(){
	myI2C2_ER_IRQHandler();
}                /* I2C2 Error                   */
void SPI1_IRQHandler(){}                   /* SPI1                         */
void SPI2_IRQHandler(){}                   /* SPI2                         */
void USART1_IRQHandler(){}                 /* USART1                       */
void USART2_IRQHandler(){}                 /* USART2                       */
void USART3_IRQHandler(){}                 /* USART3                       */
void EXTI15_10_IRQHandler(){
	if(EXTI_GetITStatus(EXTI_Line14)!=RESET){
		EXTI_ClearITPendingBit(EXTI_Line14);
		myEXTI14_IRQHandler();
	}
	if(EXTI_GetITStatus(EXTI_Line10)!=RESET){
		EXTI_ClearITPendingBit(EXTI_Line10);
		myEXTI14_IRQHandler();
	}
	if(EXTI_GetITStatus(EXTI_Line11)!=RESET){
		EXTI_ClearITPendingBit(EXTI_Line11);
		myEXTI14_IRQHandler();
	}
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET){
		EXTI_ClearITPendingBit(EXTI_Line12);
		myEXTI14_IRQHandler();
	}
	if(EXTI_GetITStatus(EXTI_Line13)!=RESET){
		EXTI_ClearITPendingBit(EXTI_Line13);
		myEXTI14_IRQHandler();
	}
}              /* External Line[15:10]s        */
void RTC_Alarm_IRQHandler(){}              /* RTC Alarm (A and B) through EXTI Line */
void OTG_FS_WKUP_IRQHandler(){}            /* USB OTG FS Wakeup through EXTI line */
void TIM8_BRK_TIM12_IRQHandler(){}         /* TIM8 Break and TIM12         */
void TIM8_UP_TIM13_IRQHandler(){}          /* TIM8 Update and TIM13        */
void TIM8_TRG_COM_TIM14_IRQHandler(){}     /* TIM8 Trigger and Commutation and TIM14 */
void TIM8_CC_IRQHandler(){}                /* TIM8 Capture Compare         */
void FSMC_IRQHandler(){}                   /* FSMC                         */
void SDIO_IRQHandler(){}                   /* SDIO                         */
void TIM5_IRQHandler(){}                   /* TIM5                         */
void SPI3_IRQHandler(){}                   /* SPI3                         */
void UART4_IRQHandler(){}                  /* UART4                        */
void UART5_IRQHandler(){}                  /* UART5                        */
void TIM6_DAC_IRQHandler(){}               /* TIM6 and DAC1&2 underrun errors */
void TIM7_IRQHandler(){}                   /* TIM7                         */
void DMA2_Stream0_IRQHandler(){}           /* DMA2 Stream 0                */
void DMA2_Stream1_IRQHandler(){}           /* DMA2 Stream 1                */
void DMA2_Stream2_IRQHandler(){}           /* DMA2 Stream 2                */
void DMA2_Stream3_IRQHandler(){}           /* DMA2 Stream 3                */
void DMA2_Stream4_IRQHandler(){}           /* DMA2 Stream 4                */
void ETH_IRQHandler(){}                    /* Ethernet                     */
void ETH_WKUP_IRQHandler(){}               /* Ethernet Wakeup through EXTI line */
void CAN2_TX_IRQHandler(){}                /* CAN2 TX                      */
void CAN2_RX0_IRQHandler(){}               /* CAN2 RX0                     */
void CAN2_RX1_IRQHandler(){}               /* CAN2 RX1                     */
void CAN2_SCE_IRQHandler(){}               /* CAN2 SCE                     */
void OTG_FS_IRQHandler(){}                 /* USB OTG FS                   */
void DMA2_Stream5_IRQHandler(){}           /* DMA2 Stream 5                */
void DMA2_Stream6_IRQHandler(){}           /* DMA2 Stream 6                */
void DMA2_Stream7_IRQHandler(){}           /* DMA2 Stream 7                */
void USART6_IRQHandler(){}                 /* USART6                       */
void I2C3_EV_IRQHandler(){}                /* I2C3 event                   */
void I2C3_ER_IRQHandler(){}                /* I2C3 error                   */
void OTG_HS_EP1_OUT_IRQHandler(){}         /* USB OTG HS End Point 1 Out   */
void OTG_HS_EP1_IN_IRQHandler(){}          /* USB OTG HS End Point 1 In    */
void OTG_HS_WKUP_IRQHandler(){}            /* USB OTG HS Wakeup through EXTI */
void OTG_HS_IRQHandler(){}                 /* USB OTG HS                   */
void DCMI_IRQHandler(){}                   /* DCMI                         */
void CRYP_IRQHandler(){}                   /* CRYP crypto                  */
void HASH_RNG_IRQHandler(){}               /* Hash and Rng                 */
void FPU_IRQHandler(){}                    /* FPU                          */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
