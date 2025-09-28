/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-20 14:35:22
 * @LastEditTime : 2025-09-26 16:31:11
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#ifndef __ENCODER_H__
#define __ENCODER_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "myMain.h"
#include "../inc/interrupt.h"

typedef enum
{
    ENCODER_DIRECTION_STOP = 0,
    ENCODER_DIRECTION_FORWARD,
    ENCODER_DIRECTION_BACKWARD
} encoder_direction_t;

typedef struct 
{
    int32_t count;
    int32_t count_last;
    int32_t speed;
    encoder_direction_t  direction;
    interrupt_gpio_t * gpio;
} encoder_t;

int encoder_init(void);

extern encoder_t encoder[4];

#ifdef __cplusplus
}
#endif


#endif /* __ENCODER_H__ */

