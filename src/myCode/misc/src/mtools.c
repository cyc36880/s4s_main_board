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

