#include "udc_init.h"



int main()
{
    udc_init();

    uint8_t data[] = {1};
    udc_pack_append_data(&pack, 2, sizeof(data), data);

    udc_pack_task();
    return 0;
}
