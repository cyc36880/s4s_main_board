#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__


#ifdef __cplusplus
extern "C" {
#endif


#include "../../myMain.h"

typedef struct _interrupt_time_    interrupt_timer_t;
typedef struct _interrupt_time_01_ interrupt_timer_01_t;
typedef struct _interrupt_gpio_    interrupt_gpio_t;

typedef void (*interrupt_call_func_t)(interrupt_timer_t * i_timer);
typedef void (*interrupt_01_call_func_t)(interrupt_timer_01_t * i_timer_01);
typedef void (*interrupt_gpio_func_t)(interrupt_gpio_t * i_gpio);

struct _interrupt_time_
{
    uint32_t count_ms; // contador de tiempo en ms
    uint32_t period;   // periodo de interrupción en ms
    interrupt_call_func_t call_func; // función a ejecutar
    void * param;
};

struct _interrupt_time_01_
{
    interrupt_01_call_func_t call_func;
    void * user_data;
};

struct _interrupt_gpio_
{
    uint32_t pin; 
    interrupt_gpio_func_t call_func; // función a ejecutar
    void * param;
};

void interrupt_init(void);
interrupt_timer_t * interrupt_timer_new(interrupt_call_func_t func, uint32_t period, void * param, uint32_t timeout);
interrupt_timer_01_t * interrupt_timer_01_new(interrupt_01_call_func_t func, void * user_data, uint32_t timeout);
interrupt_gpio_t * interrupt_gpio_new(uint32_t pin, interrupt_gpio_func_t func, void * param, uint32_t timeout);

uint32_t time_get_01_tick(void);

#ifdef __cplusplus
}
#endif


#endif /* __INTERRUPT_H__ */

