#pragma once
#include "main.h"

#include "mpu6050_typedef.h"
#include "mpu6050_const.h"

/* Low-level I2C functions */

MPU_Status_t MPU6050_WriteRegister_I2C(I2C_HandleTypeDef *hi2c, \
    uint8_t register_addr, uint8_t value);

MPU_Status_t MPU6050_ReadRegister_I2C(I2C_HandleTypeDef *hi2c, uint8_t register_addr, uint8_t *pData);