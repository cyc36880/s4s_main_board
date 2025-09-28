/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-20 14:35:22
 * @LastEditTime : 2025-09-26 09:09:40
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/interrupt.h"

#include "tim.h"
#include "gpio.h"
#include "i2c.h"


/******************
 * data type
 *****************/
#define LOCK   1
#define UNLOCK 0


/****************************
 * static function
 ***************************/


/******************
 * static variable
 *****************/
static volatile uint8_t gpio_lock = UNLOCK;
static cc_list_t * gpio_list = NULL;


/******************
 * global variable
 *****************/
ztimer_t i_time_1ms = {0};


/**************************
 * global functions
 ************************/
void interrupt_init(void)
{
    zst_timer_core_init(&i_time_1ms);
    cc_list_new(&gpio_list);
}


interrupt_gpio_t * interrupt_gpio_new(uint32_t pin, interrupt_gpio_func_t func, void * param, uint32_t timeout)
{
    uint32_t last_tick = zst_tick_get();
    if (NULL == gpio_list) return NULL;

    if (LOCK == gpio_lock)
    {
        while (1)
        {
            if (zst_tick_elaps(last_tick) < timeout)
            {
                if (UNLOCK == gpio_lock)
                    break;
            }
            else
            {
                return NULL;
            }
        }
    }
    // 加锁
    gpio_lock = LOCK;

    interrupt_gpio_t * gpio = (interrupt_gpio_t *)zst_mem_calloc(1, sizeof(interrupt_gpio_t));
    if (NULL == gpio)
    {
        gpio_lock = UNLOCK;
        return NULL;
    }

    gpio->param = param;
    gpio->call_func = func;
    gpio->pin = pin;

    cc_list_insert_tail(gpio_list, gpio);

    // 解锁
    gpio_lock = UNLOCK;

    return gpio;
}



/**
 * GPIO回调函数
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (NULL == gpio_list) return;
    if (0==cc_list_size(gpio_list) || LOCK==gpio_lock) return;
    gpio_lock = LOCK;

    interrupt_gpio_t * interrupt_gpio;
    cc_list_node_t * first_node = gpio_list->root.next;
    uint16_t size_count = 0;
    
    while (gpio_list->root.size != size_count)
    {
        size_count++;
        interrupt_gpio = (interrupt_gpio_t *)first_node->data;
        first_node = first_node->next;
        if (interrupt_gpio->pin == GPIO_Pin)
        {
            if (interrupt_gpio->call_func)
            {
                interrupt_gpio->call_func(interrupt_gpio);
            }
        }
    }
    gpio_lock = UNLOCK;
}

void HAL_IncTick(void)
{
  uwTick += uwTickFreq;
  zst_timer_handler(&i_time_1ms);
}
