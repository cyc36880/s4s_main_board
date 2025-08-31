/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-27 15:14:24
 * @LastEditTime : 2025-08-31 14:22:48
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/light_ambient.h"

#include "../../drive/inc/ws2812.h"
#include "../inc/i2c_receive.h"

/******************
 * data struct 
 *****************/
typedef struct 
{
    uint8_t light;
    union
    {
        uint8_t rgb[3];
        struct 
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
    };
} light_t;

/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask);

/********************
 * static variables
 *******************/
static uint8_t dev_state[1] = {DEV_OK};
static light_t light = {0};
// WS2812
WS2812_STATIC_BUFFER(ws2812_buffer, WS2812_M_BOARD_NUM); 
static ws2812_t ws2812_config = {
    .buffer = ws2812_buffer,  
    .length = WS2812_M_BOARD_NUM,
    .port = WS2812_M_BOARD_PORT
};
static element_data_t element_array[] = {
    [0] = {
        .name = "state",
        .data = dev_state, 
        .size = sizeof(dev_state)
    },
    [1] = {
        .name = "light",
        .data = &light.light, 
        .size = sizeof(light.light)
    },
    [2] = {
        .name = "rgb",
        .data = &light.rgb,
        .size = sizeof(light.rgb)
    }
};
static pack_data_t pack_data = {
    .name = "light_ambient",
    .elements_array = element_array,
    .size = sizeof(element_array) / sizeof(element_data_t),
};

/**************************
 * extern functions
 ************************/
void light_ambient_init(void)
{
    ptask_base_t ptask_base = {
        .run = ptask_run_callback,
    };
    ptask_1_collection.ptask_light_ambient = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);

    ws2812_init(&ws2812_config); // 初始化ws2812
    pack_data_add_list(LIGHT_AMBIENT_START_ADDR, &pack_data);
}


/****************************
 * static callback function
 ***************************/

static void ptask_run_callback(ptask_t * ptask)
{
    uint8_t data_changed=0;
    element_data_t * element_data = NULL;
    PACK_DATA_CHECK_CHANGE_FOREACH(&pack_data, element_data,
        data_changed = 1;
    );
    if (data_changed)
    {
        uint8_t r = (uint32_t)light.r * light.light / 255;
        uint8_t g = (uint32_t)light.g * light.light / 255;
        uint8_t b = (uint32_t)light.b * light.light / 255;
        ws2812_set_all_rgb(&ws2812_config, r, g, b);
    }
}



