/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "../Inc/comm.h"
/* USER CODE END Includes */

extern TIM_HandleTypeDef htim1;

extern TIM_HandleTypeDef htim2;

/* USER CODE BEGIN Private defines */
typedef enum{
	TIMER2_CH_UXCOM=0,
	TIMER2_CH_I2CS,
	TIMER2_CH_ROUND,
	TIMER2_CH_POWER_SEQ,
	TIMER2_CH_RESET,
	TIMER2_CH_POW_SET,
	TIMER2_CH_GROUP_CHANGE,
	TIMER2_CH_OCP,
	TIMER2_CH_LTC2983DELAY,
//	TIMER2_CH_OVP,
//	TIMER1_CH_POW_SET,
//	TIMER1_CH_GROUP_CHANGE,
//	TIMER1_CH_SP1,
//	TIMER1_CH_SP2,
	TIMER2_CH_TOTAL
}TIMER2_CH;


extern unsigned int T2_FLAG_COUNT[TIMER2_CH_TOTAL];
extern unsigned int T2_FLAG_EN[TIMER2_CH_TOTAL];
/* USER CODE END Private defines */

void MX_TIM1_Init(void);
void MX_TIM2_Init(void);

/* USER CODE BEGIN Prototypes */
void HAL_DelayUS(uint32_t us);
unsigned int Timer2_GetChannel(TIMER2_CH ch);
void Timer2_SetChannel(TIMER2_CH ch, unsigned int count, unsigned int en);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

