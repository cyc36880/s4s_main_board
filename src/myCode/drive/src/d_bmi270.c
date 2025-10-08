/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-27 09:59:51
 * @LastEditTime : 2025-10-08 11:24:54
 * @Description  :
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "../inc/d_bmi270.h"
#include "drive/inc/interrupt.h"
#include "component/BMI270/bmi270.h"
#include "i2c.h"
#include <math.h>
/**
 * deg/s → degree per second → 每秒多少“角度”（°）
 * rad/s → radian per second → 每秒多少“弧度”（rad）
 */

/******************
 * data type
 *****************/
#define LOG_TAG "d_bmi270"

#define READ_WRITE_LEN  UINT8_C(46)
#define GRAVITY_EARTH (9.80665f)
#define ACCEL UINT8_C(0x00)
#define GYRO UINT8_C(0x01)

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define DEG2RAD (M_PI / 180.0f)
#define RAD2DEG (180.0f / M_PI)

// Madgwick 算法增益（调节加速度的影响强度）
#define BETA 0.04f // 典型值 0.01~0.05，数值越大越快收敛，抗噪声差

/****************************
 * static function
 ***************************/
static void ptask_event_callback(ptask_t *task, ptask_event_t *e);
static void ptask_run_callback(ptask_t * ptask);
static void device_init(void);
static void timer_IMU_callback(zst_timer_t *timer);

void calibrate_gyro(int N, int delay_ms);
static void MadgwickAHRSupdateIMU(float gx, float gy, float gz,
                                  float ax, float ay, float az,
                                  float dt);
static void getEuler(float *roll, float *pitch, float *yaw);

static void bmi270_device_init(void);
static BMI2_INTF_RETURN_TYPE bmi2_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
static BMI2_INTF_RETURN_TYPE bmi2_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
static void bmi2_delay_us(uint32_t period, void *intf_ptr);
static void bmi270_read_accel(float *ax, float *ay, float *az);
static void bmi270_read_gyro(float *gx, float *gy, float *gz);
static int8_t set_accel_gyro_config(struct bmi2_dev *bmi);
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width);
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width);


/******************
 * static variable
 *****************/
static zst_timer_t  * bmi_timer = NULL;
static uint8_t dev_addr = BMI2_I2C_PRIM_ADDR;
static struct bmi2_dev bmi270 = {0};
static uint8_t init_success = 0; // 初始化成功标志
static struct bmi2_sens_data sensor_data = {{0}}; // 传感器数据

static float gx_bias = 0, gy_bias = 0, gz_bias = 0;      // 陀螺零偏 (deg/s)
static float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f; // 四元数


/******************
 * global variable
 *****************/
d_bmi270_t d_bmi270 = {0};


/**************************
 * global functions
 ************************/
void d_bmi270_init(void)
{
    /***************
     * 创建任务
     **************/
    ptask_1_collection.ptask_d_bmi270 = ptask_create(ptask_root_1_collection.ptask_root_1, ptask_event_callback, NULL);
    if (NULL == ptask_1_collection.ptask_d_bmi270)
        ZST_LOGE(LOG_TAG, "create ptask failed");
    else
        ZST_LOGI(LOG_TAG, "create ptask success");

    /****************
     * 创建定时器
     ***************/
    bmi_timer = zst_timer_create(&zst_ztimer, timer_IMU_callback, 10, NULL);
    if (NULL == bmi_timer)
    {
        ZST_LOGE(LOG_TAG, "create timer failed!");
    }
    else
    {
        ZST_LOGI(LOG_TAG, "create timer success!");
        zst_timer_pause(bmi_timer);
    }
}

// --------- 外部接口：测量使能 ----------
void d_bmi270_measurement_enable(uint8_t enable)
{
    if (NULL == bmi_timer) return; // 未初始化
    if (enable)
    {
        zst_timer_resume(bmi_timer);
    }
    else
    {
        zst_timer_pause(bmi_timer);
    }
}

// --------- 外部接口：归零偏航角（方法2：直接修正四元数） ----------
void d_bmi270_resetYaw(void)
{
    float roll, pitch, yaw;
    getEuler(&roll, &pitch, &yaw);

    // 计算绕Z轴的旋转（负yaw角）
    float halfYaw = (yaw * M_PI / 180.0f) / 2.0f;
    float cy = cosf(-halfYaw);
    float sy = sinf(-halfYaw);

    // 旋转操作：绕Z轴乘以修正四元数
    float nq0 = cy * q0 - sy * q3;
    float nq1 = cy * q1 - sy * q2;
    float nq2 = cy * q2 + sy * q1;
    float nq3 = cy * q3 + sy * q0;

    // 更新全局四元数
    q0 = nq0;
    q1 = nq1;
    q2 = nq2;
    q3 = nq3;

    // 归一化一下
    float norm = sqrtf(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    if (norm > 0.0f) {
        q0 /= norm;
        q1 /= norm;
        q2 /= norm;
        q3 /= norm;
    }
}

// --------- 陀螺静置校准 ----------
void calibrate_gyro(int N, int delay_ms)
{
    double sumx = 0, sumy = 0, sumz = 0;
    for (int i = 0; i < N; i++)
    {
        float gx, gy, gz;
        bmi270_read_gyro(&gx, &gy, &gz); // deg/s
        sumx += gx;
        sumy += gy;
        sumz += gz;
        HAL_Delay(delay_ms); // 比如 delay(5) -> 200Hz
    }
    gx_bias = (float)(sumx / N);
    gy_bias = (float)(sumy / N);
    gz_bias = (float)(sumz / N);
    // printf("Gyro bias: %.3f, %.3f, %.3f deg/s\n", gx_bias, gy_bias, gz_bias);
}


/**************************
 * static functions
 ************************/
static void ptask_event_callback(ptask_t *task, ptask_event_t *e)
{
    switch (ptask_get_code(e))
    {
        case PTASK_EVENT_INIT:
            if (0 == init_success)
                device_init();
            break;

        case PTASK_EVENT_RUN:
            ptask_run_callback(task);
            break;

        default:
            break;
    }
}

static void ptask_run_callback(ptask_t * ptask)
{
    
}

static void device_init(void)
{
    if (0 != HAL_I2C_IsDeviceReady(&hi2c2, dev_addr << 1, 3, 200)) 
    {
        ZST_LOGE(LOG_TAG, "bmi270 device not found!");
        return;
    }
    bmi270_device_init();
    if (0 == init_success)
    {
        ZST_LOGE(LOG_TAG, "bmi270 init failed!");
        return;
    }
    ZST_LOGI(LOG_TAG, "bmi270 init success!");
    float ax, ay, az;
    bmi270_read_accel(&ax, &ay, &az);
    float roll_init  = atan2f(ay, az);
    float pitch_init = atan2f(-ax, sqrtf(ay*ay + az*az));
    float yaw_init   = 0.0f; // 无磁力计

    float cr = cosf(roll_init/2), sr = sinf(roll_init/2);
    float cp = cosf(pitch_init/2), sp = sinf(pitch_init/2);
    float cy = cosf(yaw_init/2), sy = sinf(yaw_init/2);

    q0 = cr*cp*cy + sr*sp*sy;
    q1 = sr*cp*cy - cr*sp*sy;
    q2 = cr*sp*cy + sr*cp*sy;
    q3 = cr*cp*sy - sr*sp*cy;
}

// imu 数据采集定时器回调; 降低采样率，减小运算量
static void timer_IMU_callback(zst_timer_t *timer)
{
    static uint32_t last_tick = 0;
    float ax, ay, az;             // 单位 g
    float gx_deg, gy_deg, gz_deg; // 单位 deg/s

    if (0 == init_success)
    {
        return;
    }

    // 1) 读取传感器
    bmi270_read_accel(&ax, &ay, &az);
    bmi270_read_gyro(&gx_deg, &gy_deg, &gz_deg);

    // 2) 去偏置
    gx_deg -= gx_bias;
    gy_deg -= gy_bias;
    gz_deg -= gz_bias;

    // 3) 保存原始缩放数据
    d_bmi270.acc[0] = ax; 
    d_bmi270.acc[1] = ay;
    d_bmi270.acc[2] = az;
    d_bmi270.gyro[0] = gx_deg; 
    d_bmi270.gyro[1] = gy_deg;
    d_bmi270.gyro[2] = gz_deg;

    // 4) 转换为 rad/s
    float gx = gx_deg * DEG2RAD;
    float gy = gy_deg * DEG2RAD;
    float gz = gz_deg * DEG2RAD;

    // 5) Madgwick 更新
    const float dt = zst_tick_get() - last_tick;
    last_tick = zst_tick_get();
    MadgwickAHRSupdateIMU(gx, gy, gz, ax, ay, az, dt / 1000.0f);

    // 6) 欧拉角
    float roll, pitch, yaw;
    getEuler(&roll, &pitch, &yaw);
    d_bmi270.angle[0] = roll; 
    d_bmi270.angle[1] = pitch;
    d_bmi270.angle[2] = yaw;
}

// Madgwick 更新函数 (无磁力计)
// ax,ay,az 单位 g
// gx,gy,gz 单位 rad/s
// dt: 采样周期，秒
static void MadgwickAHRSupdateIMU(float gx, float gy, float gz,
                                  float ax, float ay, float az,
                                  float dt)
{
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot0, qDot1, qDot2, qDot3;
    float _2q0, _2q1, _2q2, _2q3;
    float _4q0, _4q1, _4q2;
    float _8q1, _8q2;
    float q0q0, q1q1, q2q2, q3q3;

    // 1) 归一化加速度
    recipNorm = sqrtf(ax * ax + ay * ay + az * az);
    if (recipNorm == 0.0f)
        return;
    recipNorm = 1.0f / recipNorm;
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;

    // 2) 预计算
    _2q0 = 2.0f * q0;
    _2q1 = 2.0f * q1;
    _2q2 = 2.0f * q2;
    _2q3 = 2.0f * q3;
    _4q0 = 4.0f * q0;
    _4q1 = 4.0f * q1;
    _4q2 = 4.0f * q2;
    _8q1 = 8.0f * q1;
    _8q2 = 8.0f * q2;
    q0q0 = q0 * q0;
    q1q1 = q1 * q1;
    q2q2 = q2 * q2;
    q3q3 = q3 * q3;

    // 3) 梯度下降修正
    s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
    s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
    s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
    s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;

    recipNorm = sqrtf(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);
    if (recipNorm == 0.0f)
        return;
    recipNorm = 1.0f / recipNorm;
    s0 *= recipNorm;
    s1 *= recipNorm;
    s2 *= recipNorm;
    s3 *= recipNorm;

    // 4) 四元数微分方程
    qDot0 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz) - BETA * s0;
    qDot1 = 0.5f * (q0 * gx + q2 * gz - q3 * gy) - BETA * s1;
    qDot2 = 0.5f * (q0 * gy - q1 * gz + q3 * gx) - BETA * s2;
    qDot3 = 0.5f * (q0 * gz + q1 * gy - q2 * gx) - BETA * s3;

    // 5) 积分
    q0 += qDot0 * dt;
    q1 += qDot1 * dt;
    q2 += qDot2 * dt;
    q3 += qDot3 * dt;

    // 6) 归一化四元数
    recipNorm = 1.0f / sqrtf(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;
}

/******************
 * 工具函数
 *****************/
// 欧拉角输出（度）
static void getEuler(float *roll, float *pitch, float *yaw)
{
    float sinp = 2.0f * (q0 * q2 - q3 * q1);
    if (sinp > 1.0f)
        sinp = 1.0f;
    if (sinp < -1.0f)
        sinp = -1.0f;

    *roll = atan2f(2.0f * (q0 * q1 + q2 * q3),
                   1.0f - 2.0f * (q1 * q1 + q2 * q2)) *
            RAD2DEG;
    *pitch = asinf(sinp) * RAD2DEG;
    *yaw = atan2f(2.0f * (q0 * q3 + q1 * q2),
                  1.0f - 2.0f * (q2 * q2 + q3 * q3)) *
           RAD2DEG;
}


/*************************************************** */
static void bmi270_device_init(void)
{
    int8_t rslt;
    bmi270.intf = BMI2_I2C_INTF;
    bmi270.read = bmi2_i2c_read;
    bmi270.write = bmi2_i2c_write;
    bmi270.delay_us = bmi2_delay_us;
    bmi270.read_write_len = READ_WRITE_LEN;
    bmi270.config_file_ptr = NULL;
    rslt = bmi270_init(&bmi270);
    if (BMI2_OK != rslt) 
    {
        ZST_LOGE(LOG_TAG, "init failed : %d\n", rslt);
        return;
    }
    rslt = set_accel_gyro_config(&bmi270);
    if (BMI2_OK != rslt) 
    {
        ZST_LOGE(LOG_TAG, "set_accel_gyro_config failed : %d\n", rslt);
        return;
    }

    uint8_t sensor_list[2] = {BMI2_ACCEL, BMI2_GYRO};
    rslt = bmi2_sensor_enable(sensor_list, 2, &bmi270);
    if (BMI2_OK != rslt)
    {
        ZST_LOGE(LOG_TAG, "bmi2_sensor_enable failed : %d\n", rslt);
        return;
    }
    init_success = 1;
}
static BMI2_INTF_RETURN_TYPE bmi2_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    return HAL_I2C_Mem_Read(&hi2c2, dev_addr << 1, reg_addr, I2C_MEMADD_SIZE_8BIT, reg_data, len, 10);
}

static BMI2_INTF_RETURN_TYPE bmi2_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    return HAL_I2C_Mem_Write(&hi2c2, dev_addr << 1, reg_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t *)reg_data, len, 10);
}

static void bmi2_delay_us(uint32_t period, void *intf_ptr)
{
    uint32_t tick = period * 240;
	while(tick!=0) {tick--;}
}

/*!
 * @brief This internal API is used to set configurations for accel and gyro.
 */
static int8_t set_accel_gyro_config(struct bmi2_dev *bmi)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define accelerometer and gyro configuration. */
    struct bmi2_sens_config config[2];

    /* Configure the type of feature. */
    config[ACCEL].type = BMI2_ACCEL;
    config[GYRO].type = BMI2_GYRO;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi2_get_sensor_config(config, 2, bmi);
//    bmi2_error_codes_print_result(rslt);

    /* Map data ready interrupt to interrupt pin. */
    rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, bmi);
//    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[ACCEL].cfg.acc.odr = BMI2_ACC_ODR_200HZ;

        /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
        config[ACCEL].cfg.acc.range = BMI2_ACC_RANGE_2G;

        /* The bandwidth parameter is used to configure the number of sensor samples that are averaged
         * if it is set to 2, then 2^(bandwidth parameter) samples
         * are averaged, resulting in 4 averaged samples.
         * Note1 : For more information, refer the datasheet.
         * Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but
         * this has an adverse effect on the power consumed.
         */
        config[ACCEL].cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;

        /* Enable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         * For more info refer datasheet.
         */
        config[ACCEL].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

        /* The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[GYRO].cfg.gyr.odr = BMI2_GYR_ODR_200HZ;

        /* Gyroscope Angular Rate Measurement Range.By default the range is 2000dps. */
        config[GYRO].cfg.gyr.range = BMI2_GYR_RANGE_2000;

        /* Gyroscope bandwidth parameters. By default the gyro bandwidth is in normal mode. */
        config[GYRO].cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;

        /* Enable/Disable the noise performance mode for precision yaw rate sensing
         * There are two modes
         *  0 -> Ultra low power mode(Default)
         *  1 -> High performance mode
         */
        config[GYRO].cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;

        /* Enable/Disable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         */
        config[GYRO].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;

        /* Set the accel and gyro configurations. */
        rslt = bmi2_set_sensor_config(config, 2, bmi);
        // bmi2_error_codes_print_result(rslt);
    }

    return rslt;
}

/*!
 * @brief This function converts lsb to meter per second squared for 16 bit accelerometer at
 * range 2G, 4G, 8G or 16G.
 */
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (GRAVITY_EARTH * val * g_range) / half_scale;
}

/*!
 * @brief This function converts lsb to degree per second for 16 bit gyro at
 * range 125, 250, 500, 1000 or 2000dps.
 */
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (dps / (half_scale)) * (val);
}


static void bmi270_read_accel(float *ax, float *ay, float *az)
{
    int rslt = bmi2_get_sensor_data(&sensor_data, &bmi270);
    if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_ACC) &&
        (sensor_data.status & BMI2_DRDY_GYR))
    {
        /* Converting lsb to meter per second squared for 16 bit accelerometer at 2G range. */
        *ax = lsb_to_mps2(sensor_data.acc.x, (float)2, bmi270.resolution);
        *ay = lsb_to_mps2(sensor_data.acc.y, (float)2, bmi270.resolution);
        *az = lsb_to_mps2(sensor_data.acc.z, (float)2, bmi270.resolution);
    }
}

static void bmi270_read_gyro(float *gx, float *gy, float *gz)
{
    int rslt = bmi2_get_sensor_data(&sensor_data, &bmi270);
    if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_ACC) &&
        (sensor_data.status & BMI2_DRDY_GYR))
    {
        /* Converting lsb to degree per second for 16 bit gyro at 2000dps range. */
        *gx = lsb_to_dps(sensor_data.gyr.x, (float)2000, bmi270.resolution);
        *gy = lsb_to_dps(sensor_data.gyr.y, (float)2000, bmi270.resolution);
        *gz = lsb_to_dps(sensor_data.gyr.z, (float)2000, bmi270.resolution);
    }
}
