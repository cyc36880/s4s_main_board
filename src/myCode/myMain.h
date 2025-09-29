/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-14 16:50:18
 * @LastEditTime : 2025-09-29 16:10:40
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#ifndef __MYMAIN_H__
#define __MYMAIN_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "component/zs_tools/src/zst_core.h"
#include "sys/inc/sys.h"
#include "sys/inc/delay.h"
#include "misc/inc/mtools.h"

#include "stdio.h"
#include "string.h"

typedef enum
{
    DEV_OK = 0,
    DEV_ERROR,
} DEV_STATE;

typedef struct 
{
    ptask_t * ptask_root_1;
} ptask_root_1_collection_t;

typedef struct 
{
    ptask_t * ptask_gray;
    ptask_t * ptask_ultr;
    ptask_t * ptask_mrtc;
    ptask_t * ptask_light_ambient; // 氛围灯
    ptask_t * ptask_power;         // 电源管理
    ptask_t * ptask_servo;         // 舵机
    ptask_t * ptask_encoder_motor; // 编码电机
    ptask_t * ptask_gyro;          // 陀螺仪
    ptask_t * ptask_voice;         // 语音
    
    ptask_t * ptask_i2c_receive;   // I2C 接收
} ptask_1_collection_t;


// i2c 各个功能起始地址
#define I2C_DEVICE_ADDR_SPACE(I2CDEV, space)      I2CDEV ## _SPACE = I2CDEV+space-1

enum I2C_DEVICE_ADDR
{
    LIGHT_POWER_START_ADDR = 0x00,  // 充电管理 （是否在充电、电量）
    I2C_DEVICE_ADDR_SPACE(LIGHT_POWER_START_ADDR, 5),
     
    LIGHT_AMBIENT_START_ADDR,       // 氛围灯 （亮度; 颜色）
    I2C_DEVICE_ADDR_SPACE(LIGHT_AMBIENT_START_ADDR, 5),

    RTC_START_ADDR,                 // RTC （时间、日期）
    I2C_DEVICE_ADDR_SPACE(RTC_START_ADDR, 5),

    SERVO_MOTOR_START_ADDR,         // 舵机 x2
    I2C_DEVICE_ADDR_SPACE(SERVO_MOTOR_START_ADDR, 5),

    GYRO_START_ADDR,                 // 陀螺仪
    I2C_DEVICE_ADDR_SPACE(GYRO_START_ADDR, 10),

    VOICE_START_ADDR,                // 语音
    I2C_DEVICE_ADDR_SPACE(VOICE_START_ADDR, 5),

    ENCODER_MOTOR0_START_ADDR = 80, // 编码电机0
    I2C_DEVICE_ADDR_SPACE(ENCODER_MOTOR0_START_ADDR, 10),
    ENCODER_MOTOR1_START_ADDR,      // 编码电机1
    I2C_DEVICE_ADDR_SPACE(ENCODER_MOTOR1_START_ADDR, 10),
    ENCODER_MOTOR2_START_ADDR,      // 编码电机2
    I2C_DEVICE_ADDR_SPACE(ENCODER_MOTOR2_START_ADDR, 10),
    ENCODER_MOTOR3_START_ADDR,      // 编码电机3
    I2C_DEVICE_ADDR_SPACE(ENCODER_MOTOR3_START_ADDR, 10),

    I2C_DEVICE_TITLE,
};
// 氛围灯
#define WS2812_M_BOARD_PORT 0
#define WS2812_M_BOARD_NUM  3
// 电量指示灯
#define WS2812_POWER_PORT   1
#define WS2812_POWER_NUM    1

extern ptask_root_1_collection_t ptask_root_1_collection;
extern ptask_1_collection_t ptask_1_collection;
extern cc_hash_map_t * pack_data_list;

void setup(void);
void loop(void);

#ifdef __cplusplus
}
#endif


#endif /* __MYMAIN_H__ */

