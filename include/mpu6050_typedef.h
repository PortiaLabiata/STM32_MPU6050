#pragma once
#include "main.h"

typedef enum {
    MPU_STATUS_OK,
    MPU_STATUS_I2C_ERROR
} MPU_Status_t;

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
    ACCEL_CONFIG = 0x1C
} MPU_Register_t;