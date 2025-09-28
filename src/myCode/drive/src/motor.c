/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-26 14:48:20
 * @LastEditTime : 2025-09-27 19:58:34
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/motor.h"
#include "misc/inc/mtools.h"
#include "gpio.h"
#include "tim.h"

/******************
 * data type
 *****************/
#define LOG_TAG "motor"

typedef struct 
{
    GPIO_TypeDef * dir_gpio_port;
    uint16_t dir_gpio_pin;
    uint8_t power_channel;
} motorTypedef;


/****************************
 * static function
 ***************************/
static const motorTypedef motor_array[] = {
    {motor0_dir_GPIO_Port, motor0_dir_Pin, TIM_CHANNEL_1},
    {motor1_dir_GPIO_Port, motor1_dir_Pin, TIM_CHANNEL_2},
    {motor2_dir_GPIO_Port, motor2_dir_Pin, TIM_CHANNEL_3},
    {motor3_dir_GPIO_Port, motor3_dir_Pin, TIM_CHANNEL_4},
};


/******************
 * static variable
 *****************/


/******************
 * global variable
 *****************/
motor_t motor[4] = {0};


/**************************
 * global functions
 ************************/
void motor_init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
    for (int i = 0; i < sizeof(motor) / sizeof(motor[0]); i++)
    {
        motor[i].channel = i;
        motor[i].power = 0;
    }
}


void motor_set_speed(motor_t * motor, int16_t power)
{
    if (NULL == motor) return;
    power = myMax(-1000, myMin(1000, power));
    motor->power = power;
    __HAL_TIM_SET_COMPARE(&htim2, motor_array[motor->channel].power_channel, motor->power);
    HAL_GPIO_WritePin(motor_array[motor->channel].dir_gpio_port, motor_array[motor->channel].dir_gpio_pin, \
        motor->power >= 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
