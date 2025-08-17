/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-15 13:58:48
 * @LastEditTime : 2025-08-15 14:42:47
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/pwm.h"

#include "tim.h"

typedef struct 
{
    TIM_HandleTypeDef *htim;
    uint32_t Channel;
} time_pwm_dma_t;

static const time_pwm_dma_t time_pwm_dma[] = {
    [0] = {
        .htim = &htim1,
        .Channel = TIM_CHANNEL_1,
    },
    [1] = {
        .htim = &htim1,
        .Channel = TIM_CHANNEL_2,
    }
};


void pwm_init(void)
{
    // 开启pwm输出
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
}

/**
 * @description: 设置 pwm 占空比
 * @param {uint16_t} port 0-1
 * @param {uint32_t} channel 0-3
 * @param {uint16_t} duty_cycle 0-99
 * @return {*}
 */
int pwm_set_duty_cycle(uint16_t port, uint32_t channel, uint16_t duty_cycle)
{
    if (port > 1) return -1;
    if (channel > 3) return -2;
    duty_cycle = (duty_cycle > 99) ? 99 : duty_cycle;

}


