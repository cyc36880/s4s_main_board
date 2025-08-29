/*
 *                        _oo0oo_
 *                       o8888888o
 *                       88" . "88
 *                       (| -_- |)
 *                       0\  =  /0
 *                     ___/`---'\___
 *                   .' \\|     |// '.
 *                  / \\|||  :  |||// \
 *                 / _||||| -:- |||||- \
 *                |   | \\\  - /// |   |
 *                | \_|  ''\---/''  |_/ |
 *                \  .-\__  '-'  ___/-. /
 *              ___'. .'  /--.--\  `. .'___
 *           ."" '<  `.___\_<|>_/___.' >' "".
 *          | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *          \  \ `_.   \_ __\ /__ _/   .-` /  /
 *      =====`-.____`.___ \_____/___.-`___.-'=====
 *                        `=---='
 * 
 * 
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *            佛祖保佑     永不宕机     永无BUG
 */

/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-14 16:50:18
 * @LastEditTime : 2025-08-28 11:07:43
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "./myMain.h"
#include "./BSP/inc/BSP_INCLUDE.h"
#include "./Drive/inc/hardware.h"
#include "i2c.h"

/*********************
 * extern variables
 ********************/
ptask_root_1_collection_t ptask_root_1_collection = {0};
ptask_1_collection_t ptask_1_collection = {0};


/********************
 * extern functions
 *******************/

void setup(void)
{
    delay_init(72);

    // I2C_Slave_Reset(); // 开启i2c从机模式
    HAL_I2C_EnableListen_IT(&hi2c1);

    zst_init();      // 初始化 zst
    hardware_init(); // 系统硬件驱动初始化
    ptask_root_1_collection.ptask_root_1 = ptask_root_create(&zst_ptask_list, NULL); // 创建根任务
    
    // BSP 初始化
    i2c_receive_init();   // i2c 接收（控制器）初始化

    // gray_init(); // 直接由主机控制
    // ultr_init();

    light_ambient_init(); // 氛围灯
    power_init();         // 电源管理
    encode_motor_init();  // 编码电机
    mRTC_init();          // RTC
    servo_init();         // 舵机
}

void loop(void)
{
    zst_task_handler();
}


