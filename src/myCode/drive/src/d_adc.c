#include "../inc/d_adc.h"

#include "adc.h"

#define ADC_MAX_VALUE     4095
#define ADC_MIN_VALUE     0
#define ADC_BUFFER_SIZE   1

static uint16_t adc_val_buffer[ADC_BUFFER_SIZE] = {0};

void d_adc_init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_val_buffer, ADC_BUFFER_SIZE);
}


d_adc_t * d_adc_create(uint8_t channel)
{
    d_adc_t * d_adc = (d_adc_t *)zst_mem_calloc(1, sizeof(d_adc_t));
    if (d_adc)
    {
        d_adc->buffer = &adc_val_buffer[0];
    }
    return d_adc;
}

uint32_t d_adc_get_val(d_adc_t * adc)
{
    if (adc && adc->buffer)
    {
        return *(adc->buffer);
    }
    return 0;
}

uint32_t d_adc_get_val_range(d_adc_t * adc, uint32_t min, uint32_t max)
{
    uint32_t adc_value = d_adc_get_val(adc);
    printf("adc_value: %d\n", adc_value);
    if (min > max) return 0;
    // 把adc_value映射到[min, max]区间
    return number_map(adc_value, ADC_MIN_VALUE, ADC_MAX_VALUE+1, min, max+1);
}

