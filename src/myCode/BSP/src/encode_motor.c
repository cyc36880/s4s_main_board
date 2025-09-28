/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-20 14:35:22
 * @LastEditTime : 2025-09-28 17:50:15
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/encode_motor.h"

#include "misc/inc/mtools.h"
#include "drive/inc/encoder.h"
#include "drive/inc/motor.h"
#include "drive/inc/interrupt.h"
#include "../inc/i2c_receive.h"

/********************
 * macro definitions 
 *******************/
#define LOG_TAG "encode_motor"

#define ENCODER_MOTOR_FUNC_NUM  6 // 单个编码电机功能数量


/******************
 * data struct 
 *****************/
typedef enum 
{
    ENCODER_MOTOR_MODE_NORMAL = 0,
    ENCODER_MOTOR_MODE_POSITION,
    ENCODER_MOTOR_MODE_SPEED,
}ENCODER_MOTOR_MODE;

typedef struct 
{
    uint8_t mode;               // 参考ENCODER_MOTOR_MODE
    uint8_t power[2];           // 电机功率 (-1000 ~ 1000)
    uint8_t target_position[4]; // 目标位置
    uint8_t encoder_value[4];   // 当前编码器数值
    int8_t  position_pid[3];    // 位置pid
    int8_t  speed_pid[3];       // 速度pid

    zst_pid_t position_pid_handle; // 位置 pid 计算
    zst_pid_t speed_pid_handle;    // 速度 pid 计算

} device_t;


/****************************
 * static function
 ***************************/
static void encoder_motor_timer_callback(zst_timer_t *timer);
static void ptask_run_callback(ptask_t * ptask);


/********************
 * static variables
 *******************/
static device_t device[ ENCODER_MOTOR_NUM ] = {0};
static element_data_t element_array[ ENCODER_MOTOR_NUM ] [ ENCODER_MOTOR_FUNC_NUM ] = {0};
static pack_data_t pack_data[ ENCODER_MOTOR_NUM ] = {
    [0] = {
        .name = "encoder_motor_0",
        .elements_array = element_array[0],
        .size = sizeof(element_array[0]) / sizeof(element_data_t),
    },
    [1] = {
        .name = "encoder_motor_1",
        .elements_array = element_array[1],
        .size = sizeof(element_array[1]) / sizeof(element_data_t),
    },
    [2] = {
        .name = "encoder_motor_2",
        .elements_array = element_array[2],
        .size = sizeof(element_array[2]) / sizeof(element_data_t),
    },
    [3] = {
        .name = "encoder_motor_3",
        .elements_array = element_array[3],
        .size = sizeof(element_array[3]) / sizeof(element_data_t),
    },
};


/**************************
 * global functions
 ************************/
void encode_motor_init(void)
{
    for (uint8_t emi = 0; emi < ENCODER_MOTOR_NUM; emi++)
    {
        element_data_t * element_array_ptr = pack_data[emi].elements_array;
        element_array_ptr[0].name = "mode";
        element_array_ptr[0].data = &device[emi].mode;
        element_array_ptr[0].size = sizeof(device[emi].mode);

        element_array_ptr[1].name = "power";
        element_array_ptr[1].data = device[emi].power;
        element_array_ptr[1].size = sizeof(device[emi].power);

        element_array_ptr[2].name = "target_position";
        element_array_ptr[2].data = device[emi].target_position;
        element_array_ptr[2].size = sizeof(device[emi].target_position);

        element_array_ptr[3].name = "encoder_value";
        element_array_ptr[3].data = device[emi].encoder_value;
        element_array_ptr[3].size = sizeof(device[emi].encoder_value);

        element_array_ptr[4].name = "position_pid";
        element_array_ptr[4].data = device[emi].position_pid;
        element_array_ptr[4].size = sizeof(device[emi].position_pid);

        element_array_ptr[5].name = "speed_pid";
        element_array_ptr[5].data = device[emi].speed_pid;
        element_array_ptr[5].size = sizeof(device[emi].speed_pid);


        pid_init(&device[emi].position_pid_handle, INT32_MAX, 0, 0, 0);
        pid_init(&device[emi].speed_pid_handle, 1000, 0, 0, 0);
    }

    ptask_base_t ptask_base = {
        .run = ptask_run_callback,
    };
    ptask_1_collection.ptask_encoder_motor = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);
    if (NULL == ptask_1_collection.ptask_encoder_motor)
        ZST_LOGE(LOG_TAG, "create ptask failed");
    else
        ZST_LOGI(LOG_TAG, "create ptask success");
    pack_data_add_list(ENCODER_MOTOR0_START_ADDR, &pack_data[0]);
    pack_data_add_list(ENCODER_MOTOR1_START_ADDR, &pack_data[1]);
    pack_data_add_list(ENCODER_MOTOR2_START_ADDR, &pack_data[2]);
    pack_data_add_list(ENCODER_MOTOR3_START_ADDR, &pack_data[3]);
    zst_timer_create(&i_time_1ms, encoder_motor_timer_callback, 50, NULL);
}


/****************************
 * static callback function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    for (uint8_t emi = 0; emi < ENCODER_MOTOR_NUM; emi++)
    {
        device_t * p_device = &device[emi];
        pack_data_t * p_pack_data = &pack_data[emi];
        element_data_t * p_element_data;
        PACK_DATA_CHECK_CHANGE_FOREACH(p_pack_data, p_element_data, 
            if (0 == strcmp(p_element_data->name, "encoder_value")) // 重置/指定 编码器的值
            {
                int32_t encoder_value = array_data_to_int32((uint8_t *)p_element_data->data);
                encoder[emi].count = encoder_value;
                if (0 == encoder_value) // 设置编码器的值为0，则重置pid的值
                {
                    p_device->speed_pid_handle.i_val = 0;
                    p_device->speed_pid_handle.last_diff = 0;

                    p_device->position_pid_handle.i_val = 0;
                    p_device->position_pid_handle.last_diff = 0;
                }
            }
            else if (0 == strcmp(p_element_data->name, "position_pid"))
            {
                int8_t * pid_array = (int8_t *)p_element_data->data;
                p_device->position_pid_handle.P = pid_array[0];
                p_device->position_pid_handle.I = pid_array[1];
                p_device->position_pid_handle.D = pid_array[2];
            }
            else if (0 == strcmp(p_element_data->name, "speed_pid"))
            {
                int8_t * pid_array = (int8_t *)p_element_data->data;
                p_device->speed_pid_handle.P = pid_array[0];
                p_device->speed_pid_handle.I = pid_array[1];
                p_device->speed_pid_handle.D = pid_array[2];
            }
        );
        // 更新编码器数值
        uint8_t * encoder_value = (uint8_t *)pack_data_get_element_4name(p_pack_data, "encoder_value")->data;
        int32_to_array_data(encoder[emi].count, encoder_value);
    }
}

// timer -- pid控制电机
static void encoder_motor_timer_callback(zst_timer_t *timer)
{
    for (uint8_t emi = 0; emi < ENCODER_MOTOR_NUM; emi++)
    {
        device_t * p_device = &device[emi];
        int16_t motor_power = 0;
        if (ENCODER_MOTOR_MODE_NORMAL == p_device->mode) // 直流电机模式
        {
            motor_power = array_data_to_int16(p_device->power);
        }
        else if (ENCODER_MOTOR_MODE_POSITION == p_device->mode) // 位置控制模式
        {
            motor_power = pid_calculation(&p_device->position_pid_handle, encoder[emi].count, array_data_to_int32(p_device->target_position));
        }
        else if (ENCODER_MOTOR_MODE_SPEED == p_device->mode) // 速度控制模式
        {
            motor_power = pid_calculation(&p_device->speed_pid_handle, encoder[emi].speed, array_data_to_int16(p_device->power));
        }
        motor_set_speed(&motor[emi], motor_power);
    }
}

