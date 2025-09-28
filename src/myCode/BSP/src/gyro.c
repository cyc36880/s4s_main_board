/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-27 10:02:59
 * @LastEditTime : 2025-09-28 17:06:59
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/gyro.h"
#include "../inc/i2c_receive.h"
#include "drive/inc/d_bmi270.h"
#include "misc/inc/mtools.h"


/******************
 * data type
 *****************/
#define LOG_TAG "gyro"

typedef struct 
{
    /**
     * 0: 禁用
     * 1：启用
     */
    uint8_t enable;

    /**
     * 0: 空闲
     * 1：校准中
     */
    uint8_t state; // 状态

    /**
     * 补码方式存储
     */
    uint8_t acc[2 * 3];  // 加速度
    uint8_t gyro[2 * 3]; // 角速度
    uint8_t angle[2 * 3];// 角度
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
        .name = "enable",
        .data = &device.enable,
        .size = sizeof(device.enable)
    },
    [1] = {
        .name = "state",
        .data = &device.state, 
        .size = sizeof(device.state)
    },
    [2] = {
        .name = "acc",
        .data = device.acc,
        .size = sizeof(device.acc)
    },
    [3] = {
        .name = "gyro",
        .data = device.gyro,
        .size = sizeof(device.gyro)
    },
    [4] = {
        .name = "angle",
        .data = device.angle,
        .size = sizeof(device.angle)
    },
};
static pack_data_t pack_data = {
    .name = "gyro",
    .elements_array = element_array,
    .size = sizeof(element_array) / sizeof(element_data_t),
};


/**************************
 * global functions
 ************************/
void gyro_init(void)
{
    ptask_base_t ptask_base = {
        .run = ptask_run_callback,
    };
    ptask_1_collection.ptask_gyro = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);
    if (NULL == ptask_1_collection.ptask_gyro)
        ZST_LOGE(LOG_TAG, "create ptask failed");
    else
        ZST_LOGI(LOG_TAG, "create ptask success");
    
    pack_data_add_list(GYRO_START_ADDR, &pack_data);
}


/****************************
 * static function
 ***************************/

/**
 * @description: 任务运行回调
 */
static void ptask_run_callback(ptask_t * ptask)
{
    element_data_t * element_data = NULL;
    PACK_DATA_CHECK_CHANGE_FOREACH(&pack_data, element_data,
        if (0 == strcmp(element_data->name, "enable"))
        {
            d_bmi270_measurement_enable(device.enable);
        }
        else if (0 == strcmp(element_data->name, "state"))
        {
            switch (device.state)
            {
                case 1:
                    calibrate_gyro(600, 5); // 600*5ms=3秒
                    break;
                case 2:
                    d_bmi270_resetYaw(); // 重置yaw
                    break;
                default:
                    break;
            }
            device.state = 0;
        }
    );
    for (uint8_t axis=0; axis<3; axis++)
    {
        int16_to_array_data(d_bmi270.acc[axis] * 100, device.acc + axis * 2);
        int16_to_array_data(d_bmi270.gyro[axis] * 100, device.gyro + axis * 2);
        int16_to_array_data(d_bmi270.angle[axis] * 100, device.angle + axis * 2);
    }
}
