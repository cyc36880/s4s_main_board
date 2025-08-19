#include "../inc/encoder.h"
#include "../../SYS/inc/sys.h"

static void interrupt_20ms(interrupt_timer_t * i_timer);
void interrupt_gpio(interrupt_gpio_t * i_gpio);

encoder_t encoder0 = {0};
encoder_t encoder1 = {0};
encoder_t encoder2 = {0};
encoder_t encoder3 = {0};

int encoder_init(void)
{
    // 创建中断定时器，更新编码器的值
    interrupt_timer_new(interrupt_20ms, 20, NULL, 0xffff);

    // 创建中断GPIO，更新编码器的值
    encoder0.gpio = interrupt_gpio_new(GPIO_PIN_13, interrupt_gpio, NULL, 0xffff);
    encoder1.gpio = interrupt_gpio_new(GPIO_PIN_15, interrupt_gpio, NULL, 0xffff);
    encoder2.gpio = interrupt_gpio_new(GPIO_PIN_11, interrupt_gpio, NULL, 0xffff);
    encoder3.gpio = interrupt_gpio_new(GPIO_PIN_14, interrupt_gpio, NULL, 0xffff);
	
	return 0;
}


/**
 * static functions
 */

// 更新编码器的参数
static void interrupt_20ms(interrupt_timer_t * i_timer)
{
    encoder_t * encoder_array[] = {&encoder0, &encoder1, &encoder2, &encoder3};
    for(int i=0; i<sizeof(encoder_array) / sizeof(encoder_array[0]); i++)
    {
        encoder_array[i]->speed = encoder_array[i]->count - encoder_array[i]->count_last;
        encoder_array[i]->direction = encoder_array[i]->speed > 0 ? ENCODER_DIRECTION_FORWARD : 
                                        encoder_array[i]->speed < 0 ? ENCODER_DIRECTION_BACKWARD : 
                                        ENCODER_DIRECTION_STOP;
        encoder_array[i]->count_last = encoder_array[i]->count;
    }
}

// gpio中断函数
void interrupt_gpio(interrupt_gpio_t * i_gpio)
{
    if(i_gpio == encoder0.gpio)
    {
        encoder0.count += PCin(14)==0 ? 1 : -1;
    }
    else if(i_gpio == encoder1.gpio)
    {
        encoder0.count += PAin(12)==0 ? 1 : -1;
    }
    else if(i_gpio == encoder2.gpio)
    {
        encoder0.count += PAin(10)==0 ? 1 : -1;
    }
    else if(i_gpio == encoder3.gpio)
    {
        encoder0.count += PBin(15)==0 ? 1 : -1;
    }
}
