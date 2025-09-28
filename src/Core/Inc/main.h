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
#define charging_Pin GPIO_PIN_13
#define charging_GPIO_Port GPIOC
#define motor2_Pin GPIO_PIN_2
#define motor2_GPIO_Port GPIOA
#define motor3_Pin GPIO_PIN_3
#define motor3_GPIO_Port GPIOA
#define servo0_Pin GPIO_PIN_6
#define servo0_GPIO_Port GPIOA
#define servo1_Pin GPIO_PIN_7
#define servo1_GPIO_Port GPIOA
#define motor3_dir_Pin GPIO_PIN_0
#define motor3_dir_GPIO_Port GPIOB
#define motor2_dir_Pin GPIO_PIN_1
#define motor2_dir_GPIO_Port GPIOB
#define motor1_dir_Pin GPIO_PIN_2
#define motor1_dir_GPIO_Port GPIOB
#define eccoder_0_Pin GPIO_PIN_12
#define eccoder_0_GPIO_Port GPIOB
#define eccoder_0_EXTI_IRQn EXTI15_10_IRQn
#define eccoder_1_Pin GPIO_PIN_13
#define eccoder_1_GPIO_Port GPIOB
#define eccoder_1_EXTI_IRQn EXTI15_10_IRQn
#define eccoder_2_Pin GPIO_PIN_14
#define eccoder_2_GPIO_Port GPIOB
#define eccoder_2_EXTI_IRQn EXTI15_10_IRQn
#define eccoder_3_Pin GPIO_PIN_15
#define eccoder_3_GPIO_Port GPIOB
#define eccoder_3_EXTI_IRQn EXTI15_10_IRQn
#define WS2812_M_BOARD_Pin GPIO_PIN_8
#define WS2812_M_BOARD_GPIO_Port GPIOA
#define WS2812_POWER_Pin GPIO_PIN_9
#define WS2812_POWER_GPIO_Port GPIOA
#define eccoder_io_3_Pin GPIO_PIN_10
#define eccoder_io_3_GPIO_Port GPIOA
#define eccoder_io_2_Pin GPIO_PIN_11
#define eccoder_io_2_GPIO_Port GPIOA
#define eccoder_io_1_Pin GPIO_PIN_12
#define eccoder_io_1_GPIO_Port GPIOA
#define motor0_Pin GPIO_PIN_15
#define motor0_GPIO_Port GPIOA
#define motor1_Pin GPIO_PIN_3
#define motor1_GPIO_Port GPIOB
#define eccoder_io_0_Pin GPIO_PIN_4
#define eccoder_io_0_GPIO_Port GPIOB
#define motor0_dir_Pin GPIO_PIN_5
#define motor0_dir_GPIO_Port GPIOB
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
