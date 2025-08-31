/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-22 09:48:56
 * @LastEditTime : 2025-08-31 14:28:38
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#ifndef __MI2C_H__
#define __MI2C_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "../../myMain.h"


typedef struct 
{
    uint8_t port;
    uint8_t addr;
    void * user_data;
} mi2c_t;

mi2c_t * mi2c_create(uint8_t port, uint8_t addr, void * user_data);
int mi2c_write_bytes(mi2c_t * dev, const uint8_t * data, size_t size);
int mi2c_read_bytes(mi2c_t * dev, uint8_t * data, size_t size);


#ifdef __cplusplus
}
#endif


#endif /* __MI2C_H__ */

