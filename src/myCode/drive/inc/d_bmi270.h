#ifndef __D_BMI270_H__
#define __D_BMI270_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "myMain.h"

typedef struct 
{
    float acc[3];  // 加速度，单位 g
    float gyro[3]; // 角速度，单位 rad/s
    float angle[3];// 角度，单位 度
} d_bmi270_t;

void d_bmi270_init(void);
void d_bmi270_measurement_enable(uint8_t enable);
void calibrate_gyro(int N, int delay_ms);
void d_bmi270_resetYaw(void);

extern d_bmi270_t d_bmi270;

#ifdef __cplusplus
}
#endif

#endif /* __D_BMI270_H__ */
