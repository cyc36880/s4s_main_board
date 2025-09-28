#include "../inc/mtools.h"

/**
 * @description:  计算星期
 * @param {uint16_t} year
 * @param {uint16_t} month
 * @param {uint16_t} day
 * @return {*}
 */
int8_t calculate_week(uint16_t year, uint16_t month, uint16_t day)
{
    int c, y, week;
    if (month == 1 || month == 2) {
        year--;
        month += 12;
    }
    c = year / 100;
    y = year % 100;

    week = y + y / 4 + c / 4 - 2 * c + (26 * (month + 1)) / 10 + day - 1;

    // 调整负数情况
    while (week < 0)
        week += 7;

    week %= 7;

    // 转换为 1=Sunday, ..., 7=Saturday
    if (week == 0)
        return 7;  // 星期六
    else
        return week;
}

size_t number_map(size_t x, size_t in_min, size_t in_max, size_t out_min, size_t out_max) 
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int myMax(int a, int b)
{
    return a > b ? a : b;
}

int myMin(int a, int b)
{
    return a < b ? a : b;
}

int myAbs(int a)
{
    return a > 0 ? a : -a;
}


int16_t array_data_to_int16(uint8_t * array)
{
    return (int16_t)(array[0] << 8 | array[1]);
}

int32_t array_data_to_int32(uint8_t * array)
{
    return (int32_t)(array[0] << 24 | array[1] << 16 | array[2] << 8 | array[3]);
}

void int16_to_array_data(int16_t data, uint8_t * array)
{
    array[0] = (data >> 8) & 0xFF;
    array[1] = data & 0xFF;
}
void int32_to_array_data(int32_t data, uint8_t * array)
{
    array[0] = (data >> 24) & 0xFF;
    array[1] = (data >> 16) & 0xFF;
    array[2] = (data >> 8) & 0xFF;
    array[3] = data & 0xFF;
}


