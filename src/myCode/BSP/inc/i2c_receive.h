/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-27 18:27:56
 * @LastEditTime : 2025-08-31 14:26:26
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#ifndef __I2C_RECEIVE_H__
#define __I2C_RECEIVE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../myMain.h"
#include "../../misc/inc/data_structure.h"

#define PACK_DATA_CHECK_CHANGE_FOREACH(p_pack_data, p_element_data, active) \
    do                                                                      \
    {                                                                       \
        for (uint16_t _i = 0; _i < (p_pack_data)->size; _i++)               \
        {                                                                   \
            p_element_data = &((p_pack_data)->elements_array[_i]);          \
            if (0 == i2crev_clean_receive_finish(p_element_data))           \
            {                                                               \
                active                                                      \
            }                                                               \
        }                                                                   \
    } while (0)

typedef enum
{
    I2C_RECEIVE_STATE_IDLE = 0,
    I2C_RECEIVE_STATE_START,
    I2C_RECEIVE_STATE_TRANSMIT,
    I2C_RECEIVE_STATE_RECEIVE,
} i2c_receive_state_t;

void i2c_receive_init(void);
void pack_data_add_list(uint32_t key, pack_data_t *pack_data);

void i2crev_set_target_addr(uint8_t addr);
uint8_t i2crev_get_target_addr(void);
uint8_t *i2crev_get_addr_ptr(void);
void i2crev_flush_target_buffer(void);

void *i2crev_get_target_buffer(void);
size_t i2crev_get_target_buffer_size(void);

void i2crev_set_state(i2c_receive_state_t state);
i2c_receive_state_t i2crev_get_state(void);
void i2crev_finish_receive(void);
int i2crev_clean_receive_finish(element_data_t *data);


#ifdef __cplusplus
}
#endif

#endif /* __I2C_RECEIVE_H__ */
