#include "../inc/ultr.h"

#include "../../drive/inc/ws2812.h"
#include "string.h"


static int ultr_get_distance(uint16_t *data);
static void ultr_run(ptask_t * ptask);

static uint8_t dev_state[1] = {DEV_OK};
static uint8_t dev_diatance[2] = {0};

static element_data_t element_array[] = {
    [0] = {
        .name = "state",
        .data = dev_state, 
        .size = sizeof(dev_state)
    },
    [1] = {
        .name = "distance",
        .data = dev_diatance, 
        .size = sizeof(dev_diatance)
    },
};

static pack_data_t pack_data = {
    .name = "ultr",
    .elements_array = element_array,
    .size = sizeof(element_array) / sizeof(element_data_t),
};

ultr_dev_t ultr_dev = {
    .data = &pack_data
};

WS2812_STATIC_BUFFER(ws2812_buffer, ULTR_WS2812_NUM); 
static ws2812_t ws2812_config = {
    .buffer = ws2812_buffer,  
    .length = ULTR_WS2812_NUM,
    .port = WS2812_ULTR_PORT
};

// 初始化
void ultr_init(void)
{
    /**
     * 初始化
     */
    ws2812_init(&ws2812_config);

    /**
     * 创建任务
     */
    ptask_base_t ptask_base = {
        .run = ultr_run,
    };
    ptask_1_collection.ptask_ultr = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);
}


/**
 * @description: 设置超声波灯光颜色
 * @param {color_rgb_t *} light_rgb 灯光颜色数组 (长度为 ULTR_WS2812_NUM)
 * @return {*}
 */
int ultr_set_light(color_rgb_t * light_rgb)
{
    for (uint16_t i = 0; i < ws2812_config.length; i++)
    {
        ws2812_set_rgb(&ws2812_config, i, light_rgb[i].r, light_rgb[i].g, light_rgb[i].b);
    }
    return 0;
}

/**
 * @description: 设置超声波单个灯光颜色
 * @param {color_rgb_t *} light_rgb
 * @return {*}
 */
int ultr_set_light_sigle(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if (index >= ws2812_config.length) return -1;
    ws2812_set_rgb(&ws2812_config, index, r, g, b);
    return 0;
}

/**
 * static functions
 */
static int ultr_get_distance(uint16_t *data)
{
    return 0;
}

// 超声波任务
static void ultr_run(ptask_t * ptask)
{
    DEV_STATE dev_state = DEV_OK;
    uint16_t distance = 0;

    if (0 != ultr_get_distance(&distance))
        dev_state = DEV_ERROR;

    element_data_t * element_state = pack_data_get_element_n(ultr_dev.data, "state");
    if (NULL != element_state)
        memcpy(element_state->data, &dev_state, element_state->size);
    if (DEV_OK != dev_state) return;

    element_data_t * element_distance = pack_data_get_element_n(ultr_dev.data, "distance");
    if (DEV_OK == dev_state && NULL != distance)
        memcpy(element_distance->data, &distance, element_distance->size);
}
