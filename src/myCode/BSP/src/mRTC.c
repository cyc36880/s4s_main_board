/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-27 10:33:23
 * @LastEditTime : 2025-08-29 17:50:31
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/mRTC.h"

#include "stdio.h"
#include "rtc.h"
#include "../inc/i2c_receive.h"

/*******************
 * static functions
 *******************/
static void ptask_mrtc_func(ptask_t * ptask);
static void timer_cb(zst_timer_t *timer);

/*******************
 * static variables
 *******************/
static uint8_t dev_state[1] = {DEV_OK};
static RTC_TimeTypeDef rtc_time = {1, 2, 3};
static RTC_DateTypeDef rtc_data = {4, 5, 6};

static element_data_t element_array[] = {
    [0] = {
        .name = "state",
        .data = dev_state, 
        .size = sizeof(dev_state)
    },
    [1] = {
        .name = "time",
        .data = &rtc_time, 
        .size = sizeof(rtc_time)
    },
    [2] = {
        .name = "data",
        .data = &rtc_data, 
        .size = sizeof(rtc_data)
    },
};

static pack_data_t pack_data = {
    .name = "rtc",
    .elements_array = element_array,
    .size = sizeof(element_array) / sizeof(element_data_t),
};

/********************
 * initialization
 *******************/

void mRTC_init(void)
{
    // ptask_base_t ptask_base = {
    //     .run = ptask_mrtc_func,
    // };
    // ptask_1_collection.ptask_mrtc = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);

    zst_timer_create(&zst_ztimer, timer_cb, 600, NULL);

    pack_data_add_list(RTC_START_ADDR, &pack_data);
}



/****************************
 * static callback functions
 ***************************/

static void timer_cb(zst_timer_t *timer)
{
    element_data_t * element_data = NULL;
    PACK_DATA_CHECK_CHANGE_FOREACH(&pack_data, element_data,
        if (0 == strcmp(element_data->name, "time"))
        {
            HAL_RTC_SetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
        }
        else if (0 == strcmp(element_data->name, "data"))
        {
            rtc_data.WeekDay = calculate_week(2000 + rtc_data.Year, rtc_data.Month, rtc_data.Date);
            HAL_RTC_SetDate(&hrtc, &rtc_data, RTC_FORMAT_BIN);
        }
    );
    
    HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &rtc_data, RTC_FORMAT_BIN);
}


static void ptask_mrtc_func(ptask_t * ptask)
{


}




