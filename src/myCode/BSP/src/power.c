/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-27 15:53:57
 * @LastEditTime : 2025-09-28 17:14:47
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/power.h"
#include "../../drive/inc/ws2812.h"
#include "../../drive/inc/d_adc.h"
#include "../inc/i2c_receive.h"

/******************
 * data type
 *****************/
#define LOG_TAG "power"

typedef struct 
{
    uint8_t is_charging;   // 是否在充电
    uint8_t battery_level; // 电池电量
} device_t;


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask);
static void set_power_light(uint8_t power);


/******************
 * static variable
 *****************/
static device_t device = {0};
static d_adc_t * adc_battery = NULL; // 电池电压采集
// WS2812
WS2812_STATIC_BUFFER(ws2812_buffer, WS2812_POWER_NUM); 
static ws2812_t ws2812_config = {
    .buffer = ws2812_buffer,  
    .length = WS2812_POWER_NUM,
    .port = WS2812_POWER_PORT
};
static element_data_t element_array[] = {
    [0] = {
        .name = "is_charging",
        .data = &device.is_charging, 
        .size = sizeof(device.is_charging)
    },
    [1] = {
        .name = "battery_level",
        .data = &device.battery_level, 
        .size = sizeof(device.battery_level)
    }
};
static pack_data_t pack_data = {
    .name = "power",
    .elements_array = element_array,
    .size = sizeof(element_array) / sizeof(element_data_t),
};


/**************************
 * global functions
 ************************/
void power_init(void)
{
    ptask_base_t ptask_base = {
        .run = ptask_run_callback,
    };
    ptask_1_collection.ptask_power = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);
    if (NULL == ptask_1_collection.ptask_power)
        ZST_LOGE(LOG_TAG, "create ptask failed");
    else
        ZST_LOGI(LOG_TAG, "create ptask success");

    adc_battery = d_adc_create(0); // 创建 adc 采集
    ws2812_init(&ws2812_config);   // 初始化ws2812
    pack_data_add_list(LIGHT_POWER_START_ADDR, &pack_data);

    set_power_light(d_adc_get_val_range(adc_battery, 0, 100));
    ws2812_set_all_rgb(&ws2812_config, 0, 0, 0);
}



/****************************
 * static function
 ***************************/

/**
 * @description: 任务运行回调
 */
static void ptask_run_callback(ptask_t * ptask)
{
    device.is_charging = PCin(13);
    device.battery_level = d_adc_get_val_range(adc_battery, 0, 100);
    set_power_light(device.battery_level);
}

/**
 * @description: 设置电源灯的亮度
 */
static void set_power_light(uint8_t power)
{
    static const color_rgb_t rgb[3] = {
        [0] = {255, 0, 0},
        [1] = {0, 0, 255},
        [2] = {0, 255, 0},
    };
    const color_rgb_t * color = &rgb[number_map(power, 0, 100 + 1, 0, 2 + 1)];
    ws2812_set_all_rgb(&ws2812_config, color->r, color->g, color->b);
}

