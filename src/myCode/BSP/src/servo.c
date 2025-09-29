/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-27 16:50:15
 * @LastEditTime : 2025-09-29 16:18:53
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/servo.h"
#include "../inc/i2c_receive.h"
#include "../../drive/inc/d_servo.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG "servo"


typedef struct 
{
    servo_t * servo;
    uint8_t angle;
} device_t;


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask);


/********************
 * static variables
 *******************/
static device_t device[2] = {0};
static element_data_t element_array[] = {
    [0] = {
        .name = "servo_0",
        .data = &device[0].angle, 
        .size = sizeof(device[0].angle)
    },
    [1] = {
        .name = "servo_1",
        .data = &device[1].angle, 
        .size = sizeof(device[1].angle)
    },
};
static pack_data_t pack_data = {
    .name = LOG_TAG,
    .elements_array = element_array,
    .size = sizeof(element_array) / sizeof(element_data_t),
};


/**************************
 * extern functions
 ************************/
void servo_init(void)
{
    ptask_base_t ptask_base = {
        .run = ptask_run_callback,
    };
    ptask_1_collection.ptask_servo = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);
    if (NULL == ptask_1_collection.ptask_servo)
        ZST_LOGE(LOG_TAG, "create ptask failed");
    else
        ZST_LOGI(LOG_TAG, "create ptask success");
        
    device[0].servo = d_servo_create(0);
    device[1].servo = d_servo_create(1);
    pack_data_add_list(SERVO_MOTOR_START_ADDR, &pack_data);
    d_servo_set_angle(device[0].servo, 0);
    d_servo_set_angle(device[1].servo, 0);
};


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    element_data_t * element_data = NULL;
    PACK_DATA_CHECK_CHANGE_FOREACH(&pack_data, element_data,
        uint8_t angle = ((uint8_t *)element_data->data)[0];
        if (0 == strcmp(element_data->name, "servo_0"))
        {
            ZST_LOG("set servo 0 angle: %d", angle);
            d_servo_set_angle(device[0].servo, angle);
        }
        else if (0 == strcmp(element_data->name, "servo_1"))
        {
            ZST_LOG("set servo 1 angle: %d", angle);
            d_servo_set_angle(device[1].servo, angle);
        }
    );
}

