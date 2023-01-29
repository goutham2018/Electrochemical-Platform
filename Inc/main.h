/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

#define TIMx                           TIM3
#define TIMx_CLK_ENABLE()              __HAL_RCC_TIM3_CLK_ENABLE()
#define TIMx_IRQn                      TIM3_IRQn
#define TIMx_IRQHandler                TIM3_IRQHandler

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
/* Private defines -----------------------------------------------------------*/
#define Sense_Mode_SC_Pin GPIO_PIN_13
#define Sense_Mode_SC_GPIO_Port GPIOC
#define Latch_Sense_Pin GPIO_PIN_0
#define Latch_Sense_GPIO_Port GPIOC
#define PG_Buck_Pin GPIO_PIN_1
#define PG_Buck_GPIO_Port GPIOC
#define BSense_Pin GPIO_PIN_2
#define BSense_GPIO_Port GPIOC
#define MTDI_ESP_Pin GPIO_PIN_3
#define MTDI_ESP_GPIO_Port GPIOC
#define EN_ESP_Pin GPIO_PIN_4
#define EN_ESP_GPIO_Port GPIOC
#define SS_Pin GPIO_PIN_5
#define SS_GPIO_Port GPIOC
#define RST_Pin GPIO_PIN_0
#define RST_GPIO_Port GPIOB
#define READY_Pin GPIO_PIN_1
#define READY_GPIO_Port GPIOB
#define M_Z_Pin GPIO_PIN_2
#define M_Z_GPIO_Port GPIOB
#define LDAC_Pin GPIO_PIN_10
#define LDAC_GPIO_Port GPIOB
#define ADC_CONVST_Pin GPIO_PIN_11
#define ADC_CONVST_Port GPIOB
#define Boot_ESP_Pin GPIO_PIN_8
#define Boot_ESP_GPIO_Port GPIOC
#define PD_Pin GPIO_PIN_9
#define PD_GPIO_Port GPIOC
#define FLASH_HOLD_Pin GPIO_PIN_3
#define FLASH_HOLD_GPIO_Port GPIOB
#define SS7_Pin GPIO_PIN_4
#define SS7_GPIO_Port GPIOB
#define SS8_Pin GPIO_PIN_5
#define SS8_GPIO_Port GPIOB
#define SS9_Pin GPIO_PIN_6
#define SS9_GPIO_Port GPIOB
#define SS10_Pin GPIO_PIN_7
#define SS10_GPIO_Port GPIOB
#define SS11_Pin GPIO_PIN_8
#define SS11_GPIO_Port GPIOB
#define BUSY_Pin GPIO_PIN_9
#define BUSY_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define SPI3_Chip_Select_DAC_Pin GPIO_PIN_8
#define SPI3_Chip_Select_DAC_GPIO_Port GPIOA
#define SPI3_Chip_Select_Pin GPIO_PIN_15
#define SPI3_Chip_Select_GPIO_Port GPIOA
#define SPI2_Chip_Select_Pin GPIO_PIN_12
#define SPI2_Chip_Select_GPIO_Port GPIOB
#define SPI1_Chip_Select_ESP_Pin GPIO_PIN_4
#define SPI1_Chip_Select_ESP_GPIO_Port GPIOA
#define ADC_CLK_Pin GPIO_PIN_6
#define ADC_CLK_GPIO_Port GPIOC
#define ADC_DATA_Pin GPIO_PIN_7
#define ADC_DATA_GPIO_Port GPIOC


#define maxTableSize 36
#define MASK_LOW_BYTE 		0x0000FF
#define MASK_MID_BYTE 		0x00FF00
#define MASK_HIGH_BYTE 		0xFF0000
#define LSB_UNUSED				2
#define DIN_REG						0x10
#define PROGRAM_PAGE			0x02
#define READ_PAGE					0x03
#define WRITE_ENABLE			0x06
#define WRITE_DISABLE			0x04
#define SOH								0x01
#define SOH2							0x36
#define STX								0x02
#define DEV_ID						0xAB
#define ETX								0x03
#define _CRC							0x00
#define _CR								0x0D

enum { TRANSFER_WAIT, TRANSFER_COMPLETE, TRANSFER_ERROR, TX_RX_COMPARE };

extern uint32_t get_millis(void);
extern double absVal(double);

void sample_channel(void);

uint32_t get_spi2_state(void);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
