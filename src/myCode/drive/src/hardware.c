#include "../inc/hardware.h"

#include "../../Drive/inc/DRIVE_INCLUDE.h"

void hardware_init(void) 
{
    pwm_init();
    interrupt_init();
    encoder_init();
}

