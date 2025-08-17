#ifndef __WS2812_H__
#define __WS2812_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "../../myMain.h"
#include "tim.h"

#define WS2812_H           60   // 1 码相对计数值
#define WS2812_L           29   // 0 码相对计数值
#define WS2812_REST        40   // 复位信号脉冲数量
#define WS2812_DATA_LEN    24   // WS2812数据长度，单个需要24个字节
#define WS2812_RST_NUM     50   // 官方复位时间为50us（40个周期），保险起见使用50个周期

#define WS2812_STATIC_BUFFER(buffer_name, num) \
    static uint16_t buffer_name[(num) * WS2812_DATA_LEN + WS2812_RST_NUM] = {0}


typedef struct 
{
    uint16_t * buffer;
    uint16_t   length;
    uint8_t    port;
} ws2812_t;

int ws2812_init(ws2812_t *config);
int ws2812_set_rgb(ws2812_t *config, uint16_t num, uint8_t r, uint8_t g, uint8_t b);
uint32_t ws2812_get_occupy_size(ws2812_t *config);

#ifdef __cplusplus
}
#endif


#endif /* __WS2812_H__ */

