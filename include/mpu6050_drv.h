#pragma once
#include "main.h"

#include "mpu6050_typedef.h"
#include "mpu6050_const.h" 

/* Magic numbers */

//#define INT8_MAX 32767

/* Low-level I2C functions */

MPU_Status_t MPU_WriteRegister_I2C(I2C_HandleTypeDef *hi2c, \
    uint8_t register_addr, uint8_t value);

MPU_Status_t MPU_ReadRegister_I2C(I2C_HandleTypeDef *hi2c, \
    uint8_t register_addr, uint8_t *pData);

MPU_Status_t MPU_ReadBytes_I2C(I2C_HandleTypeDef *hi2c, \
    MPU_Register_t register_addr, uint32_t bytes, uint8_t *pData);

/* High-level control */

MPU_Status_t MPU_Init(MPU_Handle_I2C_t *handle);

MPU_Status_t MPU_SetAccelRange(MPU_Handle_I2C_t *handle);
MPU_Status_t MPU_SetGyroRange(MPU_Handle_I2C_t *handle);

/* Reading data */

MPU_Status_t MPU_ReadAccelX_Raw(MPU_Handle_I2C_t *handle, int16_t *pData);
MPU_Status_t MPU_ReadAccelY_Raw(MPU_Handle_I2C_t *handle, int16_t *pData);
MPU_Status_t MPU_ReadAccelZ_Raw(MPU_Handle_I2C_t *handle, int16_t *pData);
MPU_Status_t MPU_ReadAccel_Raw(MPU_Handle_I2C_t *handle, int16_t *x, int16_t *y, int16_t *z);

MPU_Status_t MPU_ReadGyroX_Raw(MPU_Handle_I2C_t *handle, int16_t *pData);
MPU_Status_t MPU_ReadGyroY_Raw(MPU_Handle_I2C_t *handle, int16_t *pData);
MPU_Status_t MPU_ReadGyroZ_Raw(MPU_Handle_I2C_t *handle, int16_t *pData);
MPU_Status_t MPU_ReadGyro_Raw(MPU_Handle_I2C_t *handle, int16_t *x, int16_t *y, int16_t *z);
