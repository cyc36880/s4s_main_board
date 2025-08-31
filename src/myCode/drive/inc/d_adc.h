/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-08-29 15:27:37
 * @LastEditTime : 2025-08-31 14:28:57
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#ifndef __D_ADC_H__
#define __D_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "../../myMain.h"


typedef struct 
{
    uint16_t * buffer;
} d_adc_t;

void d_adc_init(void);

d_adc_t * d_adc_create(uint8_t channel);
uint32_t d_adc_get_val(d_adc_t * adc);
uint32_t d_adc_get_val_range(d_adc_t * adc, uint32_t min, uint32_t max);

#ifdef __cplusplus
}
#endif

#endif /* __D_ADC_H__ */
