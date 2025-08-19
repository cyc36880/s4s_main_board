#include "../inc/encode_motor.h"

#include "../../drive/inc/encoder.h"
#include "../../drive/inc/pwm.h"
#include "../../drive/inc/interrupt.h"

typedef struct 
{
    uint8_t port0 : 3;
    uint8_t port1 : 3;
    uint8_t noused_encoder : 1; // 0:encoder motor; 1:only dc motor

} encode_motor_t;

static encode_motor_t encode_motor_array[ENCODE_MOTOR_NUM] = {
    {0, 1},
    {2, 3},
    {4, 5},
    {6, 7},
};

void encode_motor_init(void)
{



}

