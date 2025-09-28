#ifndef __DATA_STRUCTURE_H__
#define __DATA_STRUCTURE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "myMain.h"

typedef struct 
{
    char * name;
    void * data;
    uint16_t size;
    uint8_t change_flag : 1;
} element_data_t;

typedef struct 
{
    char * name;
    element_data_t * elements_array;
    uint16_t size;
    void * user_data;
} pack_data_t;

typedef struct 
{
    union
    {
        struct 
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
        uint8_t rgb[3];
    };
} color_rgb_t;

element_data_t * pack_data_get_element_4name(pack_data_t * pack, const char * name);
uint16_t pack_data_get_total_size(pack_data_t * pack);


#ifdef __cplusplus
}
#endif


#endif /* __DATA_STRUCTURE_H__ */

