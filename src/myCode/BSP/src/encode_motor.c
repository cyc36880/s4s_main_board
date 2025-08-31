/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-20 14:35:22
 * @LastEditTime : 2025-08-31 14:22:34
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/encode_motor.h"

#include "../../drive/inc/encoder.h"
#include "../../drive/inc/pwm.h"
#include "../../drive/inc/interrupt.h"
#include "../inc/i2c_receive.h"

/********************
 * macro definitions 
 *******************/
#define ENCODER_MOTOR_FUNC_NUM  6 // 功能数量



/******************
 * data struct 
 *****************/
#pragma pack(push, 1) // 1字节对齐
typedef struct 
{
    uint8_t mode; // 0 直流；1 编码电机
    union
    {
        struct 
        {
            int8_t power; // 电机功率 (-100 ~ 100)
        };
        struct 
        {
            int8_t position_pid[3];  // 位置pid
            int8_t speed_pid[3];     // 速度pid
            int16_t encoder_value;   // 当前编码器数值
            int16_t target_position; // 目标位置
        };
    };
} encoder_motor_t;
#pragma pack(pop)

/****************************
 * static function
 ***************************/
static void interrupt_callback(interrupt_timer_t * i_timer);
static void ptask_run_callback(ptask_t * ptask);

/********************
 * static variables
 *******************/
static uint8_t dev_state[1] = {DEV_OK};
static encoder_motor_t encode_motor[4] = {0};
static element_data_t element_array[1 + ( ENCODER_MOTOR_FUNC_NUM ) * 4] = {
    [0] = {
        .name = "state",
        .data = dev_state, 
        .size = sizeof(dev_state)
    },
};
static pack_data_t pack_data = {
    .name = "encode_motor",
    .elements_array = element_array,
    .size = sizeof(element_array) / sizeof(element_data_t),
};


/**************************
 * extern functions
 ************************/
void encode_motor_init(void)
{
    for (uint8_t emi = 0; emi < 4; emi++)
    {
        element_data_t  * element = element_array + 1 + ENCODER_MOTOR_FUNC_NUM * emi;
        element[0].name = "mode";
        element[0].data = &encode_motor[emi].mode;
        element[0].size = sizeof(encode_motor[emi].mode);

        element[1].name = "dcpower";
        element[1].data = &encode_motor[emi].power;
        element[1].size = sizeof(encode_motor[emi].power);

        element[2].name = "position_pid";
        element[2].data = encode_motor[emi].position_pid;
        element[2].size = sizeof(encode_motor[emi].position_pid);

        element[3].name = "speed_pid";
        element[3].data = encode_motor[emi].speed_pid;
        element[3].size = sizeof(encode_motor[emi].speed_pid);

        element[4].name = "encoder_value";
        element[4].data = &encode_motor[emi].encoder_value;
        element[4].size = sizeof(encode_motor[emi].encoder_value);

        element[5].name = "target_position";
        element[5].data = &encode_motor[emi].target_position;
        element[5].size = sizeof(encode_motor[emi].target_position);
    }

    ptask_base_t ptask_base = {
        .run = ptask_run_callback,
    };
    ptask_1_collection.ptask_encoder_motor = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);
    pack_data_add_list(ENCODER_MOTOR_START_ADDR, &pack_data);
    interrupt_timer_new(interrupt_callback, 20, NULL, 0xffff);
}


/****************************
 * static callback function
 ***************************/
static void interrupt_callback(interrupt_timer_t * i_timer)
{

}


static void ptask_run_callback(ptask_t * ptask)
{

}
