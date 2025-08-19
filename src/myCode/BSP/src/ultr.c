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

int ultr_set_light_all(uint8_t r, uint8_t g, uint8_t b)
{
    for (uint16_t i = 0; i < ws2812_config.length; i++)
    {
        ws2812_set_rgb(&ws2812_config, i, r, g, b);
    }
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

    element_data_t * element = pack_data_get_element_n(ultr_dev.data, "state");
    if (NULL != element)
        memcpy(element->data, &dev_state, element->size);
    if (DEV_OK != dev_state) return;

    element = pack_data_get_element_n(ultr_dev.data, "distance");
    if (NULL != element)
        memcpy(element->data, &distance, element->size);
}
