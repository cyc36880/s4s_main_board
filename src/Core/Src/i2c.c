/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
#include "string.h"
#include "BSP/inc/i2c_receive.h"
#include "drive/inc/interrupt.h"
#include "zs_tool.h"

#define LOG_TAG "i2c"


static zst_timer_t * i2c_timer_1ms_handler = NULL; // 1ms 定时器
static void i2c_timer_1ms_callback(zst_timer_t *timer);

static volatile uint8_t no_stop_count = 0; // 连续非停止状态计数
/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 30;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
}
/* I2C2 init function */
void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = I2C1_SCL_WITH_HAT_Pin|I2C1_SDA_WITH_HAT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_I2C1_ENABLE();

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

    /* I2C1 interrupt Init */
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
  else if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C2 clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
  /* USER CODE BEGIN I2C2_MspInit 1 */

  /* USER CODE END I2C2_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(I2C1_SCL_WITH_HAT_GPIO_Port, I2C1_SCL_WITH_HAT_Pin);

    HAL_GPIO_DeInit(I2C1_SDA_WITH_HAT_GPIO_Port, I2C1_SDA_WITH_HAT_Pin);

    /* I2C1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
  else if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();

    /**I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11);

  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */



//I2C引脚强制拉高，释放总线
static void I2C_Slave_Forced_UP()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	HAL_GPIO_WritePin(GPIOB, I2C1_SCL_WITH_HAT_Pin|I2C1_SDA_WITH_HAT_Pin, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = I2C1_SCL_WITH_HAT_Pin|I2C1_SDA_WITH_HAT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(I2C1_SCL_WITH_HAT_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(I2C1_SCL_WITH_HAT_GPIO_Port, I2C1_SCL_WITH_HAT_Pin|I2C1_SDA_WITH_HAT_Pin, GPIO_PIN_SET);
}

static void I2C_Slave_Reset(void)//初始化I2C从机接收态
{
  __HAL_RCC_I2C1_FORCE_RESET();   // 对 I2C1 外设寄存器复位
  __HAL_RCC_I2C1_RELEASE_RESET(); // 释放复位，让 I2C1 重新初始化

	HAL_I2C_DeInit(&hi2c1);		// 注销I2C
  memset(&hi2c1, 0, sizeof(hi2c1));
  memset(&hi2c1.Init, 0, sizeof(hi2c1.Init));
	HAL_I2C_MspDeInit(&hi2c1);	// 注销引脚
	I2C_Slave_Forced_UP(); // I2C引脚强制拉高，释放总线
  HAL_I2C_MspInit(&hi2c1);		// 重新初始化引脚
  MX_I2C1_Init();
	HAL_I2C_EnableListen_IT(&hi2c1);
  i2crev_set_state(I2C_RECEIVE_STATE_IDLE);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  ZST_LOGE(LOG_TAG, "error");
  I2C_Slave_Reset();
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)  //监听中断回调
{
  if (NULL == i2c_timer_1ms_handler)
  {
    i2c_timer_1ms_handler = zst_timer_create(&i_time_1ms, i2c_timer_1ms_callback, 1, NULL);
  }
  ZST_LOGD(LOG_TAG, "listen");
  //@note 有时listen后i2c也会死机，只能每次都重启
  I2C_Slave_Reset();
  no_stop_count = 0;
}


void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
  UNUSED(hi2c);
  UNUSED(AddrMatchCode);
	// 发送数据
	if(TransferDirection == I2C_DIRECTION_RECEIVE)
	{
    ZST_LOGD(LOG_TAG, "transmit");
    switch (i2crev_get_state())
    {
      case I2C_RECEIVE_STATE_START:
      {
        i2crev_flush_target_buffer(); // 刷新目标缓冲区
        void * buffer = i2crev_get_target_buffer();
        size_t size = i2crev_get_target_buffer_size();
        ZST_LOGD(LOG_TAG, "%p %d", buffer, size);
        HAL_I2C_Slave_Seq_Transmit_IT(hi2c, buffer, size, I2C_LAST_FRAME);
        i2crev_set_state(I2C_RECEIVE_STATE_TRANSMIT);
      } break;

      default:
        ;
    }
	}
	// 接收数据
	else if(TransferDirection == I2C_DIRECTION_TRANSMIT)
	{
    ZST_LOGD(LOG_TAG, "receive");
    switch (i2crev_get_state())
    {
      case I2C_RECEIVE_STATE_IDLE:
        ZST_LOGD(LOG_TAG, "adid");
        HAL_I2C_Slave_Seq_Receive_IT(hi2c, i2crev_get_addr_ptr(), 2, I2C_FIRST_AND_NEXT_FRAME);
        i2crev_set_state(I2C_RECEIVE_STATE_START);
      break;
    }
	}
}


void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)  //全部发送完成回调
{
  UNUSED(hi2c);
  ZST_LOGD(LOG_TAG, "TxCplt");
  switch (i2crev_get_state())
  {
    case I2C_RECEIVE_STATE_TRANSMIT : 
      i2crev_set_state(I2C_RECEIVE_STATE_IDLE);
      no_stop_count = 0;
    break;

    default:
      i2crev_set_state(I2C_RECEIVE_STATE_IDLE);
      no_stop_count = 0;
  }
}


void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)  //全部接收完成回调
{
  UNUSED(hi2c);
  ZST_LOGD(LOG_TAG, "RXCplt");
  i2crev_flush_target_buffer(); // 刷新目标缓冲区
  switch (i2crev_get_state())
  {
    case I2C_RECEIVE_STATE_START:
    {
      const uint16_t rev_size = i2crev_get_target_buffer_size();
      ZST_LOGD(LOG_TAG, "rev_size: %d", rev_size);
      *(uint8_t *)i2crev_get_target_buffer() = i2crev_get_addr_ptr()[1];
      if (rev_size > 1)
      {
        HAL_I2C_Slave_Seq_Receive_IT(hi2c, (uint8_t *)i2crev_get_target_buffer()+1, rev_size-1, I2C_LAST_FRAME);
        i2crev_set_state(I2C_RECEIVE_STATE_RECEIVE);
      }
      else
      {
        i2crev_finish_receive();
        i2crev_set_state(I2C_RECEIVE_STATE_IDLE);
        no_stop_count = 0;
      }
    }break;

    case I2C_RECEIVE_STATE_RECEIVE: // 完成一次接收
      i2crev_finish_receive();
      i2crev_set_state(I2C_RECEIVE_STATE_IDLE);
      no_stop_count = 0;
    break;

    default:
      i2crev_set_state(I2C_RECEIVE_STATE_IDLE);
      no_stop_count = 0;
  }
}

static void i2c_timer_1ms_callback(zst_timer_t *timer)
{
  if (I2C_RECEIVE_STATE_IDLE != i2crev_get_state())
  {
    no_stop_count++;
    if (no_stop_count > 5)
    {
      ZST_LOGW(LOG_TAG, "no stop");
      no_stop_count = 0;
      I2C_Slave_Reset();
    }
  }
}
/* USER CODE END 1 */
