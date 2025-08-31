/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-15 14:06:41
 * @LastEditTime : 2025-08-31 14:27:08
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/hardware.h"

#include "../../Drive/inc/DRIVE_INCLUDE.h"

void hardware_init(void) 
{
    pwm_init();
    interrupt_init();
    encoder_init();
    d_adc_init();
    d_servo_init();
}

