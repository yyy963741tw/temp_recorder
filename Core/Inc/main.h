/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define FPGA_RST_Pin GPIO_PIN_2
#define FPGA_RST_GPIO_Port GPIOE
#define LTC2983_CS3_Pin GPIO_PIN_8
#define LTC2983_CS3_GPIO_Port GPIOI
#define LTC2983_CS1_Pin GPIO_PIN_6
#define LTC2983_CS1_GPIO_Port GPIOF
#define LTC2983_SCK1_Pin GPIO_PIN_7
#define LTC2983_SCK1_GPIO_Port GPIOF
#define LTC2983_SDO1_Pin GPIO_PIN_8
#define LTC2983_SDO1_GPIO_Port GPIOF
#define LTC2983_SDI1_Pin GPIO_PIN_9
#define LTC2983_SDI1_GPIO_Port GPIOF
#define FPGA_SPI_SCK_Pin GPIO_PIN_5
#define FPGA_SPI_SCK_GPIO_Port GPIOA
#define FPGA_SPI_MISO_Pin GPIO_PIN_6
#define FPGA_SPI_MISO_GPIO_Port GPIOA
#define RMII_CRS_DV_Pin GPIO_PIN_7
#define RMII_CRS_DV_GPIO_Port GPIOA
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXD1_GPIO_Port GPIOC
#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOB
#define USB_TX_Pin GPIO_PIN_9
#define USB_TX_GPIO_Port GPIOA
#define USB_RX_Pin GPIO_PIN_10
#define USB_RX_GPIO_Port GPIOA
#define LTC2983_SCK2_Pin GPIO_PIN_10
#define LTC2983_SCK2_GPIO_Port GPIOC
#define LTC2983_SDO2_Pin GPIO_PIN_11
#define LTC2983_SDO2_GPIO_Port GPIOC
#define LTC2983_SDI2_Pin GPIO_PIN_12
#define LTC2983_SDI2_GPIO_Port GPIOC
#define LTC2983_CS2_Pin GPIO_PIN_2
#define LTC2983_CS2_GPIO_Port GPIOD
#define FPGA_SPI_CS_Pin GPIO_PIN_3
#define FPGA_SPI_CS_GPIO_Port GPIOD
#define FPGA_SPI_MOSI_Pin GPIO_PIN_7
#define FPGA_SPI_MOSI_GPIO_Port GPIOD
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG
#define RMII_TXD1_Pin GPIO_PIN_14
#define RMII_TXD1_GPIO_Port GPIOG
#define LCD_BL_Pin GPIO_PIN_5
#define LCD_BL_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
