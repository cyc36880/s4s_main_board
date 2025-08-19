#ifndef __PWM_H__
#define __PWM_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "../../myMain.h"

void pwm_init(void);

int pwm_set_duty_cycle(uint16_t port, uint16_t duty_cycle);

#ifdef __cplusplus
}
#endif


#endif /* __PWM_H__ */

