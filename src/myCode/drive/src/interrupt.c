#include "../inc/interrupt.h"

#include "tim.h"
#include "gpio.h"
#include "i2c.h"

#define LOCK   1
#define UNLOCK 0

static volatile uint8_t tim_lock = UNLOCK;
static cc_list_t * tim_list = NULL;

static volatile uint8_t tim_01_lock = UNLOCK;
static cc_list_t * tim_01_interrupt_callback_list = NULL; // 定时器 每 0.1ms 中断回调函数列表

static volatile uint8_t gpio_lock = UNLOCK;
static cc_list_t * gpio_list = NULL;


void interrupt_init(void)
{
    // 初始化 list
    cc_list_new(&tim_list);
    cc_list_new(&gpio_list);
    cc_list_new(&tim_01_interrupt_callback_list);
    // 启动定时器中断
    HAL_TIM_Base_Start_IT(&htim2);
}


interrupt_timer_t * interrupt_timer_new(interrupt_call_func_t func, uint32_t period, void * param, uint32_t timeout)
{
    uint32_t last_tick = zst_tick_get();
    if (NULL == tim_list) return NULL;

    if (LOCK == tim_lock)
    {
        while (1)
        {
            if (zst_tick_elaps(last_tick) < timeout)
            {
                if (UNLOCK == tim_lock)
                    break;
            }
            else
            {
                return NULL;
            }
        }
    }
    // 加锁
    tim_lock = LOCK;

    interrupt_timer_t * timer = (interrupt_timer_t *)zst_mem_alloc(sizeof(interrupt_timer_t));
    if (NULL == timer)
    {
        tim_lock = UNLOCK;
        return NULL;
    }
    zst_memset_00(timer, sizeof(interrupt_timer_t));
    timer->param = param;
    timer->call_func = func;
    timer->period = period;

    cc_list_insert_tail(tim_list, timer);

    // 解锁
    tim_lock = UNLOCK;

    return timer;
}

interrupt_timer_01_t * interrupt_timer_01_new(interrupt_01_call_func_t func, void * user_data, uint32_t timeout)
{
    uint32_t last_tick = zst_tick_get();
    if (NULL == tim_01_interrupt_callback_list) return NULL;

    if (LOCK == tim_01_lock)
    {
        while (1)
        {
            if (zst_tick_elaps(last_tick) < timeout)
            {
                if (UNLOCK == tim_01_lock)
                    break;
            }
            else
            {
                return NULL;
            }
        }
    }
    // 解锁
    tim_01_lock = LOCK;

    interrupt_timer_01_t * timer_01 = (interrupt_timer_01_t *)zst_mem_alloc(sizeof(interrupt_timer_01_t));
    if (NULL == timer_01)
    {
        tim_01_lock = UNLOCK;
        return NULL;
    }
    zst_memset_00(timer_01, sizeof(interrupt_timer_01_t));
    timer_01->user_data = user_data;
    timer_01->call_func = func;

    cc_list_insert_tail(tim_01_interrupt_callback_list, timer_01);

    // 解锁
    tim_01_lock = UNLOCK;

    return timer_01;
}


interrupt_gpio_t * interrupt_gpio_new(uint32_t pin, interrupt_gpio_func_t func, void * param, uint32_t timeout)
{
    uint32_t last_tick = zst_tick_get();
    if (NULL == gpio_list) return NULL;

    if (LOCK == gpio_lock)
    {
        while (1)
        {
            if (zst_tick_elaps(last_tick) < timeout)
            {
                if (UNLOCK == gpio_lock)
                    break;
            }
            else
            {
                return NULL;
            }
        }
    }
    // 加锁
    gpio_lock = LOCK;

    interrupt_gpio_t * gpio = (interrupt_gpio_t *)zst_mem_calloc(1, sizeof(interrupt_gpio_t));
    if (NULL == gpio)
    {
        gpio_lock = UNLOCK;
        return NULL;
    }

    gpio->param = param;
    gpio->call_func = func;
    gpio->pin = pin;

    cc_list_insert_tail(gpio_list, gpio);

    // 解锁
    gpio_lock = UNLOCK;

    return gpio;
}


uint32_t time_get_01_tick(void)
{
    return TIM2->CNT;
}


static void time2_interrupt_callback(void)
{
    if (NULL == tim_01_interrupt_callback_list) return;
    if (0==cc_list_size(tim_01_interrupt_callback_list) || LOCK==tim_01_lock) return;

    tim_01_lock = LOCK;

    interrupt_timer_01_t * interrupt_timer_01;
    cc_list_node_t * first_node = tim_01_interrupt_callback_list->root.next;
    uint16_t size_count = 0;

    while (tim_01_interrupt_callback_list->root.size != size_count)
    {
        size_count++;
        interrupt_timer_01 = (interrupt_timer_01_t *)first_node->data;
        first_node = first_node->next;

        if (interrupt_timer_01->call_func)
        {
            interrupt_timer_01->call_func(interrupt_timer_01);
        }
    }

    tim_01_lock = UNLOCK;
}



/**
 * 
 *  定时器回调函数
 * 
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint8_t tim2_count=0;
    
    if (&htim2 == htim)
    {
        time2_interrupt_callback();
        if (++tim2_count == 10)
        {
            tim2_count = 0;
            if (NULL == tim_list) return;
            if (0==cc_list_size(tim_list) || LOCK==tim_lock) return;
            tim_lock = LOCK;

            interrupt_timer_t * interrupt_timer;
            cc_list_node_t * first_node = tim_list->root.next;
            uint16_t size_count = 0;

            while (tim_list->root.size != size_count)
            {
                size_count++;
                interrupt_timer = (interrupt_timer_t *)first_node->data;
                first_node = first_node->next;
                
                if (++interrupt_timer->count_ms >= interrupt_timer->period)
                {
                    interrupt_timer->count_ms = 0;
                    if (interrupt_timer->call_func)
                    {
                        interrupt_timer->call_func(interrupt_timer);
                    }
                }
            }

            tim_lock = UNLOCK;
        }
    }
}


/**
 * GPIO回调函数
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (NULL == gpio_list) return;
    if (0==cc_list_size(gpio_list) || LOCK==gpio_lock) return;
    gpio_lock = LOCK;

    interrupt_gpio_t * interrupt_gpio;
    cc_list_node_t * first_node = gpio_list->root.next;
    uint16_t size_count = 0;
    
    while (gpio_list->root.size != size_count)
    {
        size_count++;
        interrupt_gpio = (interrupt_gpio_t *)first_node->data;
        first_node = first_node->next;
        if (interrupt_gpio->pin == GPIO_Pin)
        {
            if (interrupt_gpio->call_func)
            {
                interrupt_gpio->call_func(interrupt_gpio);
            }
        }
    }
    gpio_lock = UNLOCK;
}

// i2c 错误中断
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    HAL_I2C_MspDeInit(hi2c);
    HAL_I2C_MspInit(hi2c);
}

