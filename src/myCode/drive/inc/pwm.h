/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-15 13:58:55
 * @LastEditTime : 2025-08-31 14:27:34
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#ifndef __PWM_H__
#define __PWM_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "../../myMain.h"

void pwm_init(void);

int pwm_set_duty_cycle(uint16_t port, uint16_t duty_cycle);

#ifdef __cplusplus
}
#endif


#endif /* __PWM_H__ */

