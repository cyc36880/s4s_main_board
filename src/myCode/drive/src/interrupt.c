#include "../inc/interrupt.h"

#include "tim.h"
#include "gpio.h"

#define LOCK   1
#define UNLOCK 0

static volatile uint8_t tim_lock = UNLOCK;
static cc_list_t * tim_list = NULL;
static cc_list_iter_t tim_iter;

static volatile uint8_t gpio_lock = UNLOCK;
static cc_list_t * gpio_list = NULL;
static cc_list_iter_t gpio_iter;



void interrupt_init(void)
{
    // 初始化 list
    cc_list_new(&tim_list);
    cc_list_new(&gpio_list);
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

    interrupt_timer_t * timer = (interrupt_timer_t *)zst_mem_calloc(1, sizeof(interrupt_timer_t));
    if (NULL == timer)
    {
        tim_lock = UNLOCK;
        return NULL;
    }

    timer->param = param;
    timer->call_func = func;
    timer->period = period;

    cc_list_insert_tail(tim_list, timer);

    // 解锁
    tim_lock = UNLOCK;

    return timer;
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






// 定时器回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint8_t tim2_count=0;
    
    if (&htim2 == htim)
    {
        if (++tim2_count == 10)
        {
            tim2_count = 0;
            if (NULL == tim_list) return;
            if (0==cc_list_size(tim_list) || LOCK==tim_lock) return;
            tim_lock = LOCK;

            interrupt_timer_t * interrupt_timer;
            cc_list_iter_init(&tim_iter, tim_list, 0);
            while (cc_list_iter_next(&tim_iter, (void **)&interrupt_timer, NULL))
            {
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


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (NULL == gpio_list) return;
    if (0==cc_list_size(gpio_list) || LOCK==gpio_lock) return;
    gpio_lock = LOCK;

    interrupt_gpio_t * interrupt_gpio;
    cc_list_iter_init(&gpio_iter, gpio_list, 0);
    while (cc_list_iter_next(&gpio_iter, (void **)&interrupt_gpio, NULL))
    {
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



