#include "../inc/ws2812.h"

#include "string.h"

typedef struct 
{
    TIM_HandleTypeDef *htim;
    uint32_t Channel;
} time_pwm_dma_t;

static const time_pwm_dma_t time_pwm_dma[] = {
    [0] = {
        .htim = &htim1,
        .Channel = TIM_CHANNEL_1,
    },
    [1] = {
        .htim = &htim1,
        .Channel = TIM_CHANNEL_2,
    }
};

/**
 * extern
 */

int ws2812_init(ws2812_t *config)
{
    if (NULL == config)
        return -1;
    if (config->port >= sizeof(time_pwm_dma) / sizeof(time_pwm_dma_t))
        return -2;
    TIM_HandleTypeDef *htim = time_pwm_dma[config->port].htim;
    uint32_t Channel = time_pwm_dma[config->port].Channel;

    // 清空数组
    memset(config->buffer, 0, ws2812_get_occupy_size(config));

    // 开启pwm dma 传输
    HAL_TIM_PWM_Start_DMA(htim, Channel, (uint32_t *)config->buffer, ws2812_get_occupy_size(config) / sizeof(uint16_t));

    return 0;
}

int ws2812_set_rgb(ws2812_t *config, uint16_t num, uint8_t r, uint8_t g, uint8_t b)
{
    if (NULL == config)
        return -1;
    if (num >= config->length)
        return -2;

    uint32_t index = num * (3 * 8);

    for (uint8_t i = 0; i < 8; i++)
    {
        // 填充数组
        config->buffer[index + i] = ((g << i) & (0x80)) ? WS2812_H : WS2812_L;
        config->buffer[index + i + 8] = ((r << i) & (0x80)) ? WS2812_H : WS2812_L;
        config->buffer[index + i + 16] = ((b << i) & (0x80)) ? WS2812_H : WS2812_L;
    }
    return 0;
}


uint32_t ws2812_get_occupy_size(ws2812_t *config)
{
    if (NULL == config)
        return 0;
    return (config->length * WS2812_DATA_LEN + WS2812_RST_NUM) * sizeof(uint16_t);
}

/**
 * static
 */
