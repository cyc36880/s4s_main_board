/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-27 15:14:24
 * @LastEditTime : 2025-10-08 10:05:52
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/light_ambient.h"
#include "../inc/i2c_receive.h"
#include "drive/inc/ws2812.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG "light_ambient"

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
} device_t;

/****************************
 * static function
 ***************************/
static void ptask_event_callback(ptask_t *task, ptask_event_t *e);
static void ptask_run_callback(ptask_t * ptask);

/********************
 * static variables
 *******************/
static device_t device = {0};
// WS2812
WS2812_STATIC_BUFFER(ws2812_buffer, WS2812_M_BOARD_NUM); 
static ws2812_t ws2812_config = {
    .buffer = ws2812_buffer,  
    .length = WS2812_M_BOARD_NUM,
    .port = WS2812_M_BOARD_PORT
};
static element_data_t element_array[] = {
    [0] = {
        .name = "light",
        .data = &device.light, 
        .size = sizeof(device.light)
    },
    [1] = {
        .name = "rgb",
        .data = &device.rgb,
        .size = sizeof(device.rgb)
    }
};
static pack_data_t pack_data = {
    .name = LOG_TAG,
    .elements_array = element_array,
    .size = sizeof(element_array) / sizeof(element_data_t),
};

/**************************
 * extern functions
 ************************/
void light_ambient_init(void)
{
    /***************
     * 创建任务
     **************/
    ptask_1_collection.ptask_light_ambient = ptask_create(ptask_root_1_collection.ptask_root_1, ptask_event_callback, NULL);
    if (NULL == ptask_1_collection.ptask_light_ambient)
        ZST_LOGE(LOG_TAG, "create ptask failed");
    else
        ZST_LOGI(LOG_TAG, "create ptask success");

    /***************
     * 设备初始化
     **************/
    ws2812_init(&ws2812_config); // 初始化ws2812
    pack_data_add_list(LIGHT_AMBIENT_START_ADDR, &pack_data);
    ws2812_set_all_rgb(&ws2812_config, 0, 0, 0);
}


/****************************
 * static callback function
 ***************************/
static void ptask_event_callback(ptask_t *task, ptask_event_t *e)
{
    switch (ptask_get_code(e))
    {
        case PTASK_EVENT_RUN:
            ptask_run_callback(task);
            break;
        default:
            break;
    }
}

static void ptask_run_callback(ptask_t * ptask)
{
    uint8_t data_changed=0;
    element_data_t * element_data = NULL;
    PACK_DATA_CHECK_CHANGE_FOREACH(&pack_data, element_data,
        data_changed = 1;
    );
    if (data_changed)
    {
        uint8_t r = (uint32_t)device.r * device.light / 255;
        uint8_t g = (uint32_t)device.g * device.light / 255;
        uint8_t b = (uint32_t)device.b * device.light / 255;
        ws2812_set_all_rgb(&ws2812_config, r, g, b);
    }
}



