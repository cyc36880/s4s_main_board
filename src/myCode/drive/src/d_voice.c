/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-27 16:17:00
 * @LastEditTime : 2025-10-08 09:52:33
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/d_voice.h"
#include "usart.h"

/******************
 * data type
 *****************/
#define LOG_TAG "d_voice"
#define UDC_VOICE_RECEIVE_BUF_SIZE  64
#define UDC_VOICE_TRANSMIT_BUF_SIZE 64


/****************************
 * static function
 ***************************/
static void ptask_event_callback(ptask_t *task, ptask_event_t *e);
static void ptask_run_callback(ptask_t * ptask);
static void udc_receive_finshed_event_cb(udc_event_t * e);
static int calculate_verify(const struct _udc_pack_t *pack, const uint8_t *buf, uint16_t len, uint8_t *verify);
static int send_bytes_func(const struct _udc_pack_t *pack, const uint8_t *buf, uint16_t len);


/******************
 * static variable
 *****************/
udc_pack_t udc_voice_pack;
static udc_event_dsc_t udc_voice_event_dsc = {0};
static uint8_t udc_voice_receive_buf[UDC_VOICE_RECEIVE_BUF_SIZE] = {0};
static uint8_t udc_voice_transmit_buf[UDC_VOICE_TRANSMIT_BUF_SIZE] = {0};


/******************
 * global variable
 *****************/


/**************************
 * global functions
 ************************/
void d_voice_init(void)
{
    /************************
     * udc初始化
     ***********************/
    udc_pack_init_t pack_init = {
        .pack = &udc_voice_pack,
        .header = {
            .header = "\xAA\x55",
            .header_len = 2,
        },
        .verify = {
            .calculate_verify = calculate_verify,
            .verify_len = 2,
        }
    };
    udc_pack_init(&pack_init);
    udc_pack_set_buffer_static(&udc_voice_pack, UDC_PACK_RECEIVE,  udc_voice_receive_buf,  UDC_VOICE_RECEIVE_BUF_SIZE);
    udc_pack_set_buffer_static(&udc_voice_pack, UDC_PACK_TRANSMIT, udc_voice_transmit_buf, UDC_VOICE_TRANSMIT_BUF_SIZE);
    udc_pack_set_send_bytes_func(&udc_voice_pack, send_bytes_func);
    udc_pack_add_event_cb_static(&udc_voice_pack, &udc_voice_event_dsc, udc_receive_finshed_event_cb, UDC_EVENT_PACK_RECEIVE_FINSHED, NULL);


    /************************
     * 创建任务
     ***********************/
    ptask_1_collection.ptask_d_voice = ptask_create(ptask_root_1_collection.ptask_root_1, ptask_event_callback, NULL);
    if (NULL == ptask_1_collection.ptask_d_voice)
        ZST_LOGE(LOG_TAG, "create ptask failed");
    else
        ZST_LOGI(LOG_TAG, "create ptask success");
}


/****************************
 * static function
 ***************************/
static void ptask_event_callback(ptask_t *task, ptask_event_t *e)
{
    switch (ptask_get_code(e))
    {
        case PTASK_EVENT_RUN:
            ptask_run_callback(task);
            break;
        default:
            break;
    }
}

static void ptask_run_callback(ptask_t * ptask)
{
    udc_pack_task();
}

// udc 接收完成回调
static void udc_receive_finshed_event_cb(udc_event_t * e)
{
    #if (ZST_USE_LOG > 0)
        udc_obj_t _udc_obj;
        udc_pack_t * _udc_pack = udc_event_get_target(e);
        UDC_PACK_OBJ_FOREACH(UDC_PACK_RECEIVE, _udc_pack, &_udc_obj, 
            ZST_LOG("id: %d, size : %d", _udc_obj.id, _udc_obj.size);
        );
    #endif
}

static int calculate_verify(const struct _udc_pack_t *pack, const uint8_t *buf, uint16_t len, uint8_t *verify)
{
    verify[0] = 0x55;
    verify[1] = 0xAA;
    return 0;
}

static int send_bytes_func(const struct _udc_pack_t *pack, const uint8_t *buf, uint16_t len)
{
    uart1_send_bytes(buf, len);
    return 0;
}
