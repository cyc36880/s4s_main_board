#ifndef __MTOOLS_H__
#define __MTOOLS_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "../../myMain.h"

// 计算星期
int8_t calculate_week(uint16_t year, uint16_t month, uint16_t day);

/**
 * 数值映射
 * 特别注意！！
 *  [0 - 100] -> [0 - 20]
 * 实际上，是把[0 - 99] 平均映射到了 [0-19]
 * 在100时为20，要实现预想的映射，应该为 [0-101] [0-21]
 */
size_t number_map(size_t x, size_t in_min, size_t in_max, size_t out_min, size_t out_max) ;
int myMax(int a, int b);
int myMin(int a, int b);
int myAbs(int a);
int16_t array_data_to_int16(uint8_t * array);
int32_t array_data_to_int32(uint8_t * array);
void int16_to_array_data(int16_t data, uint8_t * array);
void int32_to_array_data(int32_t data, uint8_t * array);

#ifdef __cplusplus
}
#endif


#endif /* __MTOOLS_H__ */

