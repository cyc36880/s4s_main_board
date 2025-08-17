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
#include "stm32f1xx_hal.h"

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
#define I2C2_SCL_M_BOARD_Pin GPIO_PIN_10
#define I2C2_SCL_M_BOARD_GPIO_Port GPIOB
#define I2C2_SDA_M_BOARD_Pin GPIO_PIN_11
#define I2C2_SDA_M_BOARD_GPIO_Port GPIOB
#define PWM_WS2812_M_BOARD_Pin GPIO_PIN_8
#define PWM_WS2812_M_BOARD_GPIO_Port GPIOA
#define PWM_WS2812_ULTR_Pin GPIO_PIN_9
#define PWM_WS2812_ULTR_GPIO_Port GPIOA
#define I2C1_SCL_WITH_HAT_Pin GPIO_PIN_8
#define I2C1_SCL_WITH_HAT_GPIO_Port GPIOB
#define I2C1_SDA_WITH_HAT_Pin GPIO_PIN_9
#define I2C1_SDA_WITH_HAT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
