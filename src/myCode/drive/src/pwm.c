/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : zishen
 * @Date         : 2025-08-15 13:58:48
 * @LastEditTime : 2025-08-19 22:22:54
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
    {&htim2, TIM_CHANNEL_1},
    {&htim2, TIM_CHANNEL_2},
    {&htim2, TIM_CHANNEL_3},
    {&htim2, TIM_CHANNEL_4},
    {&htim3, TIM_CHANNEL_1},
    {&htim3, TIM_CHANNEL_2},
    {&htim3, TIM_CHANNEL_3},
    {&htim3, TIM_CHANNEL_4},
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
 * @param {uint16_t} port 0-7
 * @param {uint16_t} duty_cycle 0-99
 * @return {*}
 */
int pwm_set_duty_cycle(uint16_t port, uint16_t duty_cycle)
{
    if (port >= sizeof(time_pwm_dma)/sizeof(time_pwm_dma[0])) return -1;
    duty_cycle = (duty_cycle > 99) ? 99 : duty_cycle;

    __HAL_TIM_SET_COMPARE(time_pwm_dma[port].htim, time_pwm_dma[port].Channel, duty_cycle);

    return 0;
}


