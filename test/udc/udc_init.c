#include "udc_init.h"

udc_pack_t pack = {0};
static int calculate_verify_func(const struct _udc_pack_t *pack, const uint8_t *buf, uint16_t len, uint8_t *verify);
static uint8_t pack_send_buf[100];

static int send_bytes_func(const struct _udc_pack_t *pack, const uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        printf("%02x ", buf[i]);
    }
    return len;
}

void udc_init(void)
{
    udc_pack_init_t pack_init = {
         .pack = &pack,
        .header = {
            .header = "\xAA\x55",
            .header_len = 2,
        },
        .verify = {
            .calculate_verify = calculate_verify_func,
            .verify_len = 2,
        }
    };
    udc_pack_init(&pack_init);
    udc_pack_set_buffer_static(&pack, UDC_PACK_TRANSMIT, pack_send_buf, sizeof(pack_send_buf));
    udc_pack_set_send_bytes_func(&pack, send_bytes_func);
}


static int calculate_verify_func(const struct _udc_pack_t *pack, const uint8_t *buf, uint16_t len, uint8_t *verify)
{
    verify[0] = 0x55;
    verify[1] = 0xAA;
    return 0;
}
