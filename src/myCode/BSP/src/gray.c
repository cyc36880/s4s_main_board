#include "../inc/gray.h"

#include "../../drive/inc/IIC_Port.h"
#include "string.h"

static int gray_get_data(uint8_t *data);
static void gray_run(ptask_t * ptask);

/**
 * static variables
 */
static IIC_DEVICE * gray_dev_handle = NULL; // 灰度设备

static uint8_t dev_state[1] = {DEV_OK};
static uint8_t dev_value[4] = {0};
static uint8_t dev_command[1] = {0};

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



/**
 * external functions
 */

// 初始化灰度设备
void gray_init(void)
{
    /**
     * 初始化
     */
    gray_dev_handle = get_IIC_Port_DevHandle(0);
    if (NULL == gray_dev_handle) return;
    gray_dev_handle->set_default(gray_dev_handle);
    gray_dev_handle->set_addr(gray_dev_handle, GRAY_DEVICE_ADDRESS);

    /**
     * 创建任务
     */
    ptask_base_t ptask_base = {
        .run = gray_run,
    };
    // ptask_1_collection.ptask_gray = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);

}

// 发送命令到灰度设备
int gray_send_command(uint8_t command)
{
    if (NULL == gray_dev_handle)
        return -1;
    return gray_dev_handle->writeBytes(gray_dev_handle, &command, 1);
}


/**
 * static functions
 */

// 得到四路+命令
static int gray_get_data(uint8_t *data)
{
    if (NULL == gray_dev_handle) return -1;
    return gray_dev_handle->readBytes(gray_dev_handle, data, 5);
}



// 灰度实时运行任务
static void gray_run(ptask_t * ptask)
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


