#ifndef __GRAY_H__
#define __GRAY_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "../../myMain.h"
#include "../../misc/inc/data_structure.h"

#define GRAY_DEVICE_ADDRESS 0x6A // 灰度设备地址

typedef enum
{
    GRAY_IDLE = 0,
    GRAY_COLOR_IDENTIFY,
    GRAY_GRAY_IDENTIFY,
    GRAY_BINARY_IDENTIFY,

    GRAY_GRAY_STUDY,
    GRAY_BINARY_STUDY,
    GRAY_COLOR_CLEAR_STUDY,
    GRAY_COLOR_RED_STUDY,
    GRAY_COLOR_GREEN_STUDY,
    GRAY_COLOR_BLUE_STUDY,
    GRAY_COLOR_YELLOW_STUDY,
    GRAY_COLOR_CYAN_STUDY,
    GRAY_COLOR_PUPRLE_STUDY,
    GRAY_COLOR_BLACK_STUDY,
    GRAY_COLOR_WHITE_STUDY,

    GRAY_STATE_NUM,
} GRAY_STATE;

typedef struct 
{
    pack_data_t  * data;
} gray_dev_t;

void gray_init(void);
int gray_send_command(uint8_t command);


extern gray_dev_t gray_dev;

#ifdef __cplusplus
}
#endif


#endif /* __GRAY_H__ */

