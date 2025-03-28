#pragma once
#include "main.h"

typedef enum {
    MPU_STATUS_OK,
    MPU_STATUS_I2C_ERROR,
    MPU_STATUS_ERROR
} MPU_Status_t;

typedef enum {
    MPU_CLKS_INT,
    MPU_CLKS_PLLX,
    MPU_CLKS_PLLY,
    MPU_CLKS_PLLZ,
    MPU_CLKS_EXT32K,
    MPU_CLKS_EXT19M
} MPU_ClockSource_t;

/* TODO: добавить power management */

typedef struct {
    I2C_HandleTypeDef *hi2c;
    MPU_ClockSource_t clock_source;
} MPU_Handle_I2C_t;

typedef enum {
    WHO_AM_I = 0x75,
    ACCEL_XOUT_H = 0x3B,
    ACCEL_XOUT_L = 0x3C,
    ACCEL_YOUT_H = 0x3D,
    ACCEL_YOUT_L = 0x3E,
    ACCEL_ZOUT_H = 0x3F,
    ACCEL_ZOUT_L = 0x40,

    TEMP_OUT_H = 0x41,
    TEMP_OUT_L = 0x42,

    CONFIG = 0x1A,
    GYRO_CONFIG = 0x1B,
    ACCEL_CONFIG = 0x1C,
    SMPLRT_DIV = 0x19,

    PWR_MGMT_1 = 0x6B,
    PWR_MGMT_2 = 0x6C
} MPU_Register_t;