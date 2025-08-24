#include "../inc/data_structure.h"

#include "string.h"

element_data_t * pack_data_get_element_n(pack_data_t * pack, const char * name)
{
    if (pack == NULL || name == NULL) return NULL;

    for (uint16_t i=0; i<pack->size; i++)
    {
        if (strcmp(pack->elements_array[i].name, name) == 0)
        {
            return &pack->elements_array[i];
        }
    }
    return NULL;
}

uint16_t pack_data_get_total_size(pack_data_t * pack)
{
    uint16_t total_size = 0;
    if (pack == NULL) return 0;
    for (uint16_t i=0; i<pack->size; i++)
    {
        total_size += pack->elements_array[i].size;
    }
    return total_size;
}










