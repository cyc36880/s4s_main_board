#include "../inc/gray.h"

#include "string.h"
#include "../../drive/inc/mi2c.h"

static int gray_get_data(uint8_t *data);
static void ptask_gray_run(ptask_t * ptask);

/*******************
 * static variables
 *******************/

static uint8_t dev_state[1] = {DEV_OK};
static uint8_t dev_value[4] = {0};
static uint8_t dev_command[1] = {0};

static mi2c_t * gray_mi2c;

static element_data_t element_array[] = {
    [0] = {
        .name = "state",
        .data = dev_state, 
        .size = sizeof(dev_state)
    },
    [1] = {
        .name = "value",
        .data = dev_value, 
        .size = sizeof(dev_value)
    },
    [2] = {
        .name = "command",
        .data = dev_command, 
        .size = sizeof(dev_command)
    },
};

static pack_data_t pack_data = {
    .name = "gray",
    .elements_array = element_array,
    .size = sizeof(element_array) / sizeof(element_data_t),
};

gray_dev_t gray_dev = {
    .data = &pack_data
};



/********************
 * initialization
 *******************/

void gray_init(void)
{
    /**
     * 初始化
     */
    gray_mi2c = mi2c_create(GRAY_I2C_PORT, GRAY_DEVICE_ADDRESS, NULL);

    /**
     * 创建任务
     */
    ptask_base_t ptask_base = {
        .run = ptask_gray_run,
    };
    ptask_1_collection.ptask_gray = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);
}

// 发送命令到灰度设备
int gray_send_command(uint8_t command)
{
    return mi2c_write_bytes(gray_mi2c, &command, 1);
}


/**
 * static functions
 */

// 得到四路+命令
static int gray_get_data(uint8_t *data)
{
    return mi2c_read_bytes(gray_mi2c, data, 7);
}



// 灰度实时运行任务
static void ptask_gray_run(ptask_t * ptask)
{
    uint8_t data[5] = {0};
    DEV_STATE dev_state = DEV_OK;

    if (gray_get_data(data) != 0) 
        dev_state = DEV_ERROR;
    
    element_data_t * element_state = pack_data_get_element_n(gray_dev.data, "state");
    if (element_state != NULL)
        memcpy(element_state->data, &dev_state, element_state->size);
    if (DEV_OK != dev_state) return;

    element_data_t * element_value = pack_data_get_element_n(gray_dev.data, "value");
    element_data_t * element_command = pack_data_get_element_n(gray_dev.data, "command");

    if (element_value != NULL)
        memcpy(element_value->data, data, element_value->size);
    if (element_command != NULL)
        memcpy(element_command->data, data + element_value->size, element_command->size);
}


