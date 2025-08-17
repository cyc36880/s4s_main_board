#include "myMain.h"
#include "stdio.h"
#include "./BSP/inc/BSP_INCLUDE.h"
#include "./Drive/inc/hardware.h"

ptask_root_1_collection_t ptask_root_1_collection = {0};
ptask_1_collection_t ptask_1_collection = {0};

void setup(void)
{
    hardware_init(); // 系统硬件初始化
    zst_init(); // 初始化 zst
    ptask_root_1_collection.ptask_root_1 = ptask_root_create(&zst_ptask_list, NULL); // 创建根任务

    // BSP 初始化
    gray_init();
    ultr_init();
}

void loop(void)
{
    zst_task_handler();
}
