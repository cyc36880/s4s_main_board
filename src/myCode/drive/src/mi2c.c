/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-22 09:48:56
 * @LastEditTime : 2025-08-31 14:28:27
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/mi2c.h"

#include "i2c.h"

mi2c_t * mi2c_create(uint8_t port, uint8_t addr, void * user_data)
{
    mi2c_t * dev = (mi2c_t *)zst_mem_alloc(sizeof(mi2c_t));
    if (dev)
    {
        dev->port = port;
        dev->addr = addr;
        dev->user_data = user_data;
    }
    return dev;
}


int mi2c_write_bytes(mi2c_t * dev, const uint8_t * data, size_t size)
{
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c1, dev->addr<<1, (uint8_t *)data, size, HAL_MAX_DELAY);
    return (status == HAL_OK) ? 0 : -1;
}

int mi2c_read_bytes(mi2c_t * dev, uint8_t * data, size_t size)
{
    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(&hi2c1, dev->addr<<1, data, size, HAL_MAX_DELAY);
    return (status == HAL_OK) ? 0 : -1;
}

