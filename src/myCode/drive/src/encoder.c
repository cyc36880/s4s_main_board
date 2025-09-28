/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-20 14:35:22
 * @LastEditTime : 2025-09-26 14:06:44
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/encoder.h"
#include "SYS/inc/sys.h"
#include "stdio.h"


/******************
 * data type
 *****************/
#define LOG_TAG "encoder"


/****************************
 * static function
 ***************************/
static void i_timer_20ms_callback(zst_timer_t *timer);
static void interrupt_gpio(interrupt_gpio_t * i_gpio);


/******************
 * static variable
 *****************/


/******************
 * global variable
 *****************/
encoder_t encoder[4] = {0};


/**************************
 * global functions
 ************************/
int encoder_init(void)
{
    // 创建中断定时器，更新编码器的值
    zst_timer_create(&i_time_1ms, i_timer_20ms_callback, 20, NULL);
    // 创建中断GPIO，更新编码器的值
    encoder[0].gpio = interrupt_gpio_new(GPIO_PIN_12, interrupt_gpio, NULL, 0xffff);
    encoder[1].gpio = interrupt_gpio_new(GPIO_PIN_13, interrupt_gpio, NULL, 0xffff);
    encoder[2].gpio = interrupt_gpio_new(GPIO_PIN_14, interrupt_gpio, NULL, 0xffff);
    encoder[3].gpio = interrupt_gpio_new(GPIO_PIN_15, interrupt_gpio, NULL, 0xffff);
	
	return 0;
}


/****************************
 * static function
 ***************************/
// 更新编码器的参数
static void i_timer_20ms_callback(zst_timer_t *timer)
{
    encoder_t *encoder_array[] = {&encoder[0], &encoder[1], &encoder[2], &encoder[3]};
    for (int i = 0; i < sizeof(encoder_array) / sizeof(encoder_array[0]); i++)
    {
        encoder_array[i]->speed = encoder_array[i]->count - encoder_array[i]->count_last;
        encoder_array[i]->direction = encoder_array[i]->speed > 0 ? ENCODER_DIRECTION_FORWARD : 
                                        encoder_array[i]->speed < 0 ? 
                                            ENCODER_DIRECTION_BACKWARD : ENCODER_DIRECTION_STOP;
        encoder_array[i]->count_last = encoder_array[i]->count;
    }
}

// gpio中断函数
static void interrupt_gpio(interrupt_gpio_t * i_gpio)
{
    if(i_gpio == encoder[0].gpio)
    {
        encoder[0].count += PBin(4)==0 ? 1 : -1;
    }
    else if(i_gpio == encoder[1].gpio)
    {
        encoder[1].count += PAin(12)==0 ? 1 : -1;
    }
    else if(i_gpio == encoder[2].gpio)
    {
        encoder[2].count += PAin(11)==0 ? 1 : -1;
    }
    else if(i_gpio == encoder[3].gpio)
    {
        encoder[3].count += PAin(10)==0 ? 1 : -1;
    }
}
