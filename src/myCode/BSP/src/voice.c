/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-27 11:17:57
 * @LastEditTime : 2025-09-29 16:17:07
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/voice.h"
#include "../inc/i2c_receive.h"


/******************
 * data type
 *****************/
#define LOG_TAG "voice"

typedef struct 
{
    uint8_t nol;
} device_t;


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask);


/******************
 * static variable
 *****************/
static device_t device = {0};
static element_data_t element_array[] = {
    [0] = {
        .name = "nol",
        .data = &device.nol,
        .size = sizeof(device.nol)
    }
};
static pack_data_t pack_data = {
    .name = LOG_TAG,
    .elements_array = element_array,
    .size = sizeof(element_array) / sizeof(element_data_t),
};


/**************************
 * global functions
 ************************/
void voice_init(void)
{
    ptask_base_t ptask_base = {
        .run = ptask_run_callback,
    };
    ptask_1_collection.ptask_gyro = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);
    if (NULL == ptask_1_collection.ptask_gyro)
        ZST_LOGE(LOG_TAG, "create ptask failed");
    else
        ZST_LOGI(LOG_TAG, "create ptask success");
        
    pack_data_add_list(VOICE_START_ADDR, &pack_data);
}


/****************************
 * static function
 ***************************/
/**
 * @description: 任务运行回调
 */
static void ptask_run_callback(ptask_t * ptask)
{
    
}
