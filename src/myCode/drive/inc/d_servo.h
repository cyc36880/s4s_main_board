/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-30 10:34:34
 * @LastEditTime : 2025-08-31 14:29:18
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#ifndef __D_SERVO_H__
#define __D_SERVO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../myMain.h"

typedef struct 
{
    uint8_t channel;
} servo_t;

void d_servo_init(void);
servo_t * d_servo_create(uint8_t channel);
int d_servo_set_angle(servo_t * servo, uint8_t angle);

#ifdef __cplusplus
}
#endif

#endif /* __D_SERVO_H__ */
