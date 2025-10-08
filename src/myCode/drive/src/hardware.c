/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-15 14:06:41
 * @LastEditTime : 2025-10-08 09:38:09
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/hardware.h"
#include "../../Drive/inc/DRIVE_INCLUDE.h"
#include "usart.h"

// 串口DMA接收数据
#define UDC_PACK_DMA_BUFFER 20
static uint8_t udc_voice_pack_rx_buf[UDC_PACK_DMA_BUFFER] = {0};

void hardware_init(void) 
{
    interrupt_init();
    encoder_init();
    d_adc_init();
    d_servo_init();
    motor_init();
    d_voice_init();
    d_bmi270_init();

    // uart dma
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, udc_voice_pack_rx_buf, UDC_PACK_DMA_BUFFER);
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
}

// 串口DMA空闲中断回调函数
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    static uint8_t last_rev_size = 0;
    udc_pack_receive_data(&udc_voice_pack, &udc_voice_pack_rx_buf[last_rev_size], Size - last_rev_size);
    last_rev_size = Size;
    if (UDC_PACK_DMA_BUFFER == Size)
    {
        last_rev_size = 0;
    }
}
