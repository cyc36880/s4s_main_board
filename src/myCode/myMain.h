#ifndef __MYMAIN_H__
#define __MYMAIN_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "./component/zs_tools/src/zst_core.h"
#include "./sys/inc/sys.h"
#include "./sys/inc/delay.h"
#include "stdio.h"

typedef enum
{
    DEV_OK = 0,
    DEV_ERROR,
} DEV_STATE;

typedef struct 
{
    ptask_t * ptask_root_1;
} ptask_root_1_collection_t;

typedef struct 
{
    ptask_t * ptask_gray;
    ptask_t * ptask_ultr;
} ptask_1_collection_t;



#define WS2812_M_BOARD_PORT 0
#define WS2812_ULTR_PORT    1

#define GRAY_I2C_PORT       0


extern ptask_root_1_collection_t ptask_root_1_collection;
extern ptask_1_collection_t ptask_1_collection;

void setup(void);
void loop(void);


#ifdef __cplusplus
}
#endif


#endif /* __MYMAIN_H__ */

