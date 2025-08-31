/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-27 16:50:15
 * @LastEditTime : 2025-08-31 14:23:00
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/servo.h"
#include "../inc/i2c_receive.h"
#include "../../drive/inc/d_servo.h"

/******************
 * data struct 
 *****************/


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask);


/********************
 * static variables
 *******************/
static servo_t * servo_0 = {0};
static servo_t * servo_1 = {0};

static uint8_t dev_state[1] = {DEV_OK};
static uint8_t servo1_angle[1] = {0};
static uint8_t servo2_angle[1] = {0};

static element_data_t element_array[] = {
    [0] = {
        .name = "state",
        .data = dev_state, 
        .size = sizeof(dev_state)
    },
    [1] = {
        .name = "servo_0",
        .data = servo1_angle, 
        .size = sizeof(servo1_angle)
    },
    [2] = {
        .name = "servo_1",
        .data = servo2_angle, 
        .size = sizeof(servo2_angle)
    },
};
static pack_data_t pack_data = {
    .name = "servo",
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
    servo_0 = d_servo_create(0);
    servo_1 = d_servo_create(1);
    pack_data_add_list(SERVO_MOTOR_START_ADDR, &pack_data);
}


/****************************
 * static callback function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    element_data_t * element_data = NULL;
    PACK_DATA_CHECK_CHANGE_FOREACH(&pack_data, element_data,
        uint8_t angle = ((uint8_t *)element_data->data)[0];
        if (0 == strcmp(element_data->name, "servo_0"))
        {
            d_servo_set_angle(servo_0, angle);
        }
        else if (0 == strcmp(element_data->name, "servo_1"))
        {
            d_servo_set_angle(servo_1, angle);
        }
    );
}

