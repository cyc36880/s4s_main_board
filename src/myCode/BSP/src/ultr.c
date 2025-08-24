#include "../inc/ultr.h"

#include "../../drive/inc/interrupt.h"
#include "../../drive/inc/ws2812.h"
#include "../../component/zs_tools/src/zst_core.h"
#include "string.h"


static int ultr_get_distance(uint16_t *data);
static void ptask_ultr_run(ptask_t * ptask);
static void interrupt_gpio_func(interrupt_gpio_t * i_gpio);
static void interrupt_01_call_func(interrupt_timer_01_t * i_timer_01);
static void zst_timer_cb(zst_timer_t *timer);

static uint8_t dev_state[1] = {DEV_OK};
static uint8_t dev_diatance[2] = {0};

static uint32_t gpio_overflow_count = 0;
static uint32_t gpio_high_us = 0;
static volatile uint8_t gpio_capture_falling_flag = 0;

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

// 初始化为输出
static void gpio_output_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    HAL_GPIO_DeInit(ULTR_cap_GPIO_Port, ULTR_cap_Pin);
    GPIO_InitStruct.Pin = ULTR_cap_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ULTR_cap_GPIO_Port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(ULTR_cap_GPIO_Port, ULTR_cap_Pin, GPIO_PIN_SET);
}

/* 初始化为中断
* @mode GPIO_MODE_IT_RISING; GPIO_MODE_IT_FALLING; GPIO_MODE_IT_RISING_FALLING
*/

static void gpio_interrupt_init(uint32_t mode)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    HAL_GPIO_DeInit(ULTR_cap_GPIO_Port, ULTR_cap_Pin);
    GPIO_InitStruct.Pin = ULTR_cap_Pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ULTR_cap_GPIO_Port, &GPIO_InitStruct);
}


// 初始化
void ultr_init(void)
{
    // 初始化
    ws2812_init(&ws2812_config);

    // 创建任务
    ptask_base_t ptask_base = {
        .run = ptask_ultr_run,
    };
    ptask_1_collection.ptask_ultr = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);

    // 创建 GPIO 中断
    interrupt_gpio_new(GPIO_PIN_14, interrupt_gpio_func, NULL, 0xffff);
    interrupt_timer_01_new(interrupt_01_call_func, NULL, 0xffff);
    zst_timer_create(&zst_ztimer, zst_timer_cb, 1000, NULL);


    gpio_interrupt_init(GPIO_MODE_IT_RISING_FALLING);
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
    static const uint8_t time_out = 30; // 超时
    static uint8_t start_flag = 0;
    static uint32_t start_time = 0; zst_tick_get();

    if (0 == start_flag)
    {
        gpio_overflow_count = 0;
        gpio_high_us = 0;
        gpio_capture_falling_flag = 0;

        start_flag = 1;
        start_time = zst_tick_get();
        gpio_output_init();
        PBout(14) = 1;
        delay_us(100);
        PBout(14) = 0;
        gpio_interrupt_init(GPIO_MODE_IT_RISING_FALLING);
    }
    else
    {
        if (1 == gpio_capture_falling_flag)
        {
            *data = gpio_high_us*340/2/10000;
            gpio_capture_falling_flag = 0;
        }
        if (zst_tick_elaps(start_time) > time_out)
        {
            start_flag = 0;
            gpio_output_init();
            return -1;
        }
    }

    return 0;
}

// 超声波任务
static void ptask_ultr_run(ptask_t * ptask)
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
    if (NULL != element && distance>0)
        memcpy(element->data, &distance, element->size);
}


static void zst_timer_cb(zst_timer_t *timer)
{
    printf("count val %u\n", gpio_high_us);
}

static void interrupt_gpio_func(interrupt_gpio_t * i_gpio)
{
    static uint32_t start_count = 0;
    if (1 == PBin(14))
    {
        start_count = time_get_01_tick();
        gpio_overflow_count = 0;
    }
    else
    {
        gpio_capture_falling_flag = 1;
        gpio_high_us = time_get_01_tick() - start_count + gpio_overflow_count * 100 - 1;
        gpio_output_init(); // 重新初始化 GPIO 输出
    }
}

static void interrupt_01_call_func(interrupt_timer_01_t * i_timer_01)
{
    gpio_overflow_count++;
}
