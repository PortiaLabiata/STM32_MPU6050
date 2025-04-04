#pragma once
#include "main.h"

typedef enum {
    MPU_STATUS_OK,
    MPU_STATUS_I2C_ERROR,
    MPU_STATUS_ERROR,
    MPU_STATUS_DNR_ERROR
} MPU_Status_t;

typedef enum {
    MPU_CLKS_INT = 0,
    MPU_CLKS_PLLX = 1,
    MPU_CLKS_PLLY = 2,
    MPU_CLKS_PLLZ = 3,
    MPU_CLKS_EXT32K = 4,
    MPU_CLKS_EXT19M = 5,
    MPU_CLKS_DISABLE = 7
} MPU_ClockSource_t;

typedef enum {
    MPU_ON = 1,
    MPU_OFF = 0
} MPU_ModuleState_t;

/* TODO: добавить power management */

typedef struct {
    I2C_HandleTypeDef *hi2c;
    MPU_ClockSource_t clock_source;
    MPU_ModuleState_t temp_enable;
    MPU_ModuleState_t cycle;
    uint8_t sample_rate_div;

    uint8_t acccel_range;
    uint8_t gyro_range;

    MPU_ModuleState_t accel_fifo;
    MPU_ModuleState_t gyrox_fifo;
    MPU_ModuleState_t gyroy_fifo;
    MPU_ModuleState_t gyroz_fifo;
    MPU_ModuleState_t temp_fifo;
    MPU_ModuleState_t enable_fifo;
} MPU_Handle_I2C_t;

typedef enum {
    WHO_AM_I = 0x75,
    ACCEL_XOUT_H = 0x3B,
    ACCEL_XOUT_L = 0x3C,
    ACCEL_YOUT_H = 0x3D,
    ACCEL_YOUT_L = 0x3E,
    ACCEL_ZOUT_H = 0x3F,
    ACCEL_ZOUT_L = 0x40,

    GYRO_XOUT_H = 0x43,
    GYRO_XOUT_L = 0x44,
    GYRO_YOUT_H = 0x45,
    GYRO_YOUT_L = 0x46,
    GYRO_ZOUT_H = 0x47,
    GYRO_ZOUT_L = 0x48,

    TEMP_OUT_H = 0x41,
    TEMP_OUT_L = 0x42,

    CONFIG = 0x1A,
    GYRO_CONFIG = 0x1B,
    ACCEL_CONFIG = 0x1C,
    SMPLRT_DIV = 0x19,
    FIFO_EN = 0x23,

    PWR_MGMT_1 = 0x6B,
    PWR_MGMT_2 = 0x6C,
    USER_CTRL = 0x6A,

    FIFO_COUNT_H = 0x72,
    FIFO_COUNT_L = 0x73,
    FIFO_R_W = 0x74
} MPU_Register_t;