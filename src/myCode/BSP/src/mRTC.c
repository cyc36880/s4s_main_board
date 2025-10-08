/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-27 10:33:23
 * @LastEditTime : 2025-09-29 16:17:55
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/mRTC.h"

#include "stdio.h"
#include "rtc.h"
#include "../inc/i2c_receive.h"


/*******************
 *   data struct
 ******************/
#define LOG_TAG "mRTC"

typedef struct
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef data;
} device_t;


/*******************
 * static functions
 *******************/
static void timer_callback(zst_timer_t *timer);


/*******************
 * static variables
 *******************/
static device_t device = {0};
static element_data_t element_array[] = {
    [0] = {
        .name = "time",
        .data = &device.time,
        .size = sizeof(device.time)
    },
    [1] = {
        .name = "data",
        .data = &device.data,
        .size = sizeof(device.data)
    },
};

static pack_data_t pack_data = {
    .name = LOG_TAG,
    .elements_array = element_array,
    .size = sizeof(element_array) / sizeof(element_data_t),
};


/********************
 * global functions
 *******************/
void mRTC_init(void)
{
    /***************
     * 创建定时器
     **************/
    if (NULL == zst_timer_create(&zst_ztimer, timer_callback, 450, NULL))
    {
        ZST_LOGE(LOG_TAG, "zst_timer_create failed");
    }
    else
    {
        ZST_LOGI(LOG_TAG, "zst_timer_create success");
    }

    /***************
     * 添加数据
     **************/
    pack_data_add_list(RTC_START_ADDR, &pack_data);
}


/****************************
 * static functions
 ***************************/
static void timer_callback(zst_timer_t *timer)
{
    element_data_t * element_data = NULL;
    PACK_DATA_CHECK_CHANGE_FOREACH(&pack_data, element_data,
        if (0 == strcmp(element_data->name, "time"))
        {
            HAL_RTC_SetTime(&hrtc, &device.time, RTC_FORMAT_BIN);
        }
        else if (0 == strcmp(element_data->name, "data"))
        {
            device.data.WeekDay = calculate_week(2000 + device.data.Year, device.data.Month, device.data.Date);
            HAL_RTC_SetDate(&hrtc, &device.data, RTC_FORMAT_BIN);
        }
    );
    HAL_RTC_GetTime(&hrtc, &device.time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &device.data, RTC_FORMAT_BIN);
}




