#include "../inc/servo.h"
#include "../inc/i2c_receive.h"

/******************
 * data struct 
 *****************/


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask);


/********************
 * static variables
 *******************/
static uint8_t dev_state[1] = {DEV_OK};
static uint8_t servo1_angle[1] = {0};
static uint8_t servo2_angle[1] = {0};

static element_data_t element_array[] = {
    [0] = {
        .name = "state",
        .data = dev_state, 
        .size = sizeof(dev_state)
    },
    [1] = {
        .name = "servo1",
        .data = servo1_angle, 
        .size = sizeof(servo1_angle)
    },
    [2] = {
        .name = "servo2",
        .data = servo2_angle, 
        .size = sizeof(servo2_angle)
    },
};
static pack_data_t pack_data = {
    .name = "servo",
    .elements_array = element_array,
    .size = sizeof(element_array) / sizeof(element_data_t),
};


/**************************
 * extern functions
 ************************/
void servo_init(void)
{
    ptask_base_t ptask_base = {
        .run = ptask_run_callback,
    };
    ptask_1_collection.ptask_servo = ptask_create(ptask_root_1_collection.ptask_root_1, &ptask_base);

   pack_data_add_list(SERVO_MOTOR_START_ADDR, &pack_data);
}


/****************************
 * static callback function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{

}

