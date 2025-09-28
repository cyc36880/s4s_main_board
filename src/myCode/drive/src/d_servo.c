/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-30 10:29:57
 * @LastEditTime : 2025-09-24 17:44:39
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/d_servo.h"

#include "tim.h"

#define PWM_MAX 7500
#define PWM_MIN 1500

void d_servo_init(void)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

servo_t * d_servo_create(uint8_t channel)
{
    servo_t * servo = (servo_t *)zst_mem_calloc(1, sizeof(servo_t));
    if (servo != NULL)
    {
        servo->channel = channel;
    }
    d_servo_set_angle(servo, 0);
    return servo;
}

int d_servo_set_angle(servo_t * servo, uint8_t angle)
{
    if (NULL == servo) return -1;
    angle = myMin(angle, 180);
    size_t pulse_width = number_map(angle, 0, 180 + 1, PWM_MIN, PWM_MAX + 1);
    if (0 == servo->channel)
    {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse_width);
    }
    else if (1 == servo->channel)
    {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pulse_width);
    }
    return 0;
}



