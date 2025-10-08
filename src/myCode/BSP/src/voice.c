/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-27 11:17:57
 * @LastEditTime : 2025-10-08 10:02:48
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
static void ptask_event_callback(ptask_t *task, ptask_event_t *e);
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
    /***************
     * 创建任务
     **************/
    ptask_1_collection.ptask_voice = ptask_create(ptask_root_1_collection.ptask_root_1, ptask_event_callback, NULL);
    if (NULL == ptask_1_collection.ptask_voice)
        ZST_LOGE(LOG_TAG, "create ptask failed");
    else
        ZST_LOGI(LOG_TAG, "create ptask success");
    
    /***************
     * 添加数据
     **************/
    pack_data_add_list(VOICE_START_ADDR, &pack_data);
}


/****************************
 * static function
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

/**
 * @description: 任务运行回调
 */
static void ptask_run_callback(ptask_t * ptask)
{
    
}
