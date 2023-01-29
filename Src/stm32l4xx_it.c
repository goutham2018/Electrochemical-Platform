/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
#include "main.h"
#include "stm32l4xx_it.h"
extern TIM_HandleTypeDef    TimHandle;
extern DMA_HandleTypeDef 		hdma_spi1_tx;
extern DMA_HandleTypeDef 		hdma_spi1_rx;
extern SPI_HandleTypeDef 		hspi2;
__IO uint32_t wTransferState = TRANSFER_WAIT;

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{

}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {

  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  while (1)
  {

  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  while (1)
  {

  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  while (1)
  {

  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{

}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{

}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{

}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/**
  * @brief This function handles TIM3 global interrupt.
  */

void TIMx_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle);
}

void DMA1_Channel2_IRQHandler(void){
  HAL_DMA_IRQHandler(&hdma_spi1_rx);
}

void DMA1_Channel3_IRQHandler(void){
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
}

void DMA1_Channel4_IRQHandler(void){
  HAL_DMA_IRQHandler(hspi2.hdmarx);
	wTransferState = TX_RX_COMPARE;
}

void DMA1_Channel5_IRQHandler(void){
  HAL_DMA_IRQHandler(hspi2.hdmatx);
	wTransferState = TRANSFER_COMPLETE;
}

uint32_t get_spi2_state(){
	return wTransferState;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
