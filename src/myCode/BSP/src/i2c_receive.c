/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-27 18:25:04
 * @LastEditTime : 2025-09-28 17:15:47
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/i2c_receive.h"
#include "stdlib.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG "i2c_receive"

/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask);


/********************
 * static variables
 *******************/
static cc_hash_map_t * pack_data_list = NULL;

static uint8_t target_addr[2] = {0};
static element_data_t * target_pack_data = NULL;
static i2c_receive_state_t i2c_receive_state = I2C_RECEIVE_STATE_IDLE;

/**************************
 * global functions
 ************************/
void i2c_receive_init(void)
{
    // 创建hash表
    cc_hash_map_new(&pack_data_list, 40, NULL, NULL);
    if (NULL == pack_data_list)
        ZST_LOGE(LOG_TAG, "create hash map failed");
    else
        ZST_LOGI(LOG_TAG, "create hash map success");

    ptask_base_t ptask_base = {
        .run = ptask_run_callback,
    };
    ptask_1_collection.ptask_i2c_receive = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);
    if (NULL == ptask_1_collection.ptask_i2c_receive)
        ZST_LOGE(LOG_TAG, "create ptask failed");
    else
        ZST_LOGI(LOG_TAG, "create ptask success");
}


/**
 * @description: 添加数据包到列表
 * @param {uint32_t} key
 * @param {pack_data_t} *pack_data
 * @return {*}
 */
void pack_data_add_list(uint32_t key, pack_data_t *pack_data)
{
    for (uint16_t i=0; i<pack_data->size; i++)
    {
        uintptr_t key_val = key + i;
        element_data_t * element_data = &pack_data->elements_array[i];
        if (0 == cc_hash_map_set_new( pack_data_list, (void *)key_val, (void *)element_data) )
        {
        }
        else
        {
            ZST_LOGE(LOG_TAG, "add data to list failed");
            abort();
        }
    }
}


/**
 * @description: 设置目标地址
 * @param {uint8_t} addr
 * @return {*}
 */
void i2crev_set_target_addr(uint8_t addr)
{
    target_addr[0] = addr;
}


/**
 * @description: 获取目标地址
 * @return {*}
 */
uint8_t i2crev_get_target_addr(void)
{
    return target_addr[0];
}


/**
 * @description: 获取目标地址的指针
 * @return {*}
 */
uint8_t * i2crev_get_addr_ptr(void)
{
    return target_addr;
}


/**
 * @description: 根据目标地址，刷新目标缓冲区
 * @return {*}
 */
void i2crev_flush_target_buffer(void)
{
    void * buffer = NULL;
    uint8_t addr = target_addr[0];
    if (NULL == pack_data_list) return;
    cc_hash_map_get(pack_data_list, (void *)(addr), &buffer);
    target_pack_data = (element_data_t *)buffer;
}

/**
 * @description: 获取目标地址的缓冲区
 * @return {*}
 */
void * i2crev_get_target_buffer(void)
{
    if (NULL == pack_data_list || NULL == target_pack_data) return NULL;
    return target_pack_data->data;
}


/**
 * @description: 获取目标地址的缓冲区大小
 * @return {*}
 */
size_t i2crev_get_target_buffer_size(void)
{
    if (NULL == target_pack_data) return 0;

    return target_pack_data->size;
}


/**
 * @description: 设置状态
 * @param {i2c_receive_state_t} state
 * @return {*}
 */
void i2crev_set_state(i2c_receive_state_t state)
{
    i2c_receive_state = state;
}


/**
 * @description: 获取当前状态
 * @return {*}
 */
i2c_receive_state_t i2crev_get_state(void)
{
    return i2c_receive_state;
}



/**
 * @description: 完成接收
 * @return {*}
 */
void i2crev_finish_receive(void)
{
    if (NULL != target_pack_data)
    {
        target_pack_data->change_flag = 1;
    }
}


/**
 * @description: 清理接收完成标志
 *  @note 成功清除返回0；无需清除返回1
 * @param {element_data_t *} data
 * @return {*}
 */
int i2crev_clean_receive_finish(element_data_t * data)
{
    int clean_flag = 1;
    if (data != NULL)
    {
        if (1 == data->change_flag)
        {
            clean_flag = 0;
            data->change_flag = 0;
        }
    }
    return clean_flag;
}




/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    
}

