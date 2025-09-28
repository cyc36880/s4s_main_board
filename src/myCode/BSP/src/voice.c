/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-27 11:17:57
 * @LastEditTime : 2025-09-27 19:58:00
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/voice.h"
#include "../inc/i2c_receive.h"


/******************
 * data type
 *****************/
#define LOG_TAG "voice"


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask);


/******************
 * static variable
 *****************/


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
