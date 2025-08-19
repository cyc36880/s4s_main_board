#ifndef __ULTR_H__
#define __ULTR_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "../../myMain.h"
#include "../../misc/inc/data_structure.h"

#define ULTR_WS2812_NUM  6

typedef struct 
{
    pack_data_t  * data;
} ultr_dev_t;

void ultr_init(void);
int ultr_set_light_all(uint8_t r, uint8_t g, uint8_t b);

extern ultr_dev_t ultr_dev;

#ifdef __cplusplus
}
#endif


#endif /* __ULTR_H__ */

