#ifndef __MOTOR_H__
#define __MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "myMain.h"

typedef struct
{
    uint8_t channel; 
    int16_t power; 
} motor_t;

void motor_init(void);
void motor_set_speed(motor_t * motor, int16_t power);

extern motor_t motor[4];

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H__ */
