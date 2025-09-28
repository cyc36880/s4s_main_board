/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-20 14:35:22
 * @LastEditTime : 2025-09-26 09:04:25
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__


#ifdef __cplusplus
extern "C" {
#endif


#include "myMain.h"

typedef struct _interrupt_gpio_    interrupt_gpio_t;
typedef void (*interrupt_gpio_func_t)(interrupt_gpio_t * i_gpio);



struct _interrupt_gpio_
{
    uint32_t pin; 
    interrupt_gpio_func_t call_func; // función a ejecutar
    void * param;
};

void interrupt_init(void);
interrupt_gpio_t * interrupt_gpio_new(uint32_t pin, interrupt_gpio_func_t func, void * param, uint32_t timeout);

extern ztimer_t i_time_1ms;

#ifdef __cplusplus
}
#endif


#endif /* __INTERRUPT_H__ */

