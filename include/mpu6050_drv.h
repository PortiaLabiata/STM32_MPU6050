#pragma once
#include "main.h"

#include "cli.h"
#include "mpu6050_typedef.h"
#include "mpu6050_const.h" 
#include "mpu6050_commands.h" // For usage in other projects

/* Magic numbers */

//#define INT8_MAX 32767

/* Macroses */

#define BUFFER_SHIFT(__VAR__) (buffer[__VAR__++] << 8) | buffer[__VAR__++]

/* Service functions */

char *MPU_Error2Str(MPU_Status_t status);

/* Low-level I2C functions */

MPU_Status_t MPU_WriteRegister_I2C(I2C_HandleTypeDef *hi2c, \
    uint8_t register_addr, uint8_t value);

MPU_Status_t MPU_ReadRegister_I2C(I2C_HandleTypeDef *hi2c, \
    uint8_t register_addr, uint8_t *pData);

MPU_Status_t MPU_ReadBytes_I2C(I2C_HandleTypeDef *hi2c, \
    MPU_Register_t register_addr, uint32_t bytes, uint8_t *pData);

/* High-level control */

MPU_Status_t MPU_Init(MPU_Handle_I2C_t *handle);

MPU_Status_t MPU_SetClockSource(MPU_Handle_I2C_t *handle, MPU_ClockSource_t clock);

MPU_Status_t MPU_SetAccelRange(MPU_Handle_I2C_t *handle, uint8_t range);
MPU_Status_t MPU_SetGyroRange(MPU_Handle_I2C_t *handle, uint8_t range);

/* Reading data */

MPU_Status_t MPU_ReadAccelX_Raw(MPU_Handle_I2C_t *handle, int16_t *pData);
MPU_Status_t MPU_ReadAccelY_Raw(MPU_Handle_I2C_t *handle, int16_t *pData);
MPU_Status_t MPU_ReadAccelZ_Raw(MPU_Handle_I2C_t *handle, int16_t *pData);
MPU_Status_t MPU_ReadAccel_Raw(MPU_Handle_I2C_t *handle, int16_t *x, int16_t *y, int16_t *z);

MPU_Status_t MPU_ReadGyroX_Raw(MPU_Handle_I2C_t *handle, int16_t *pData);
MPU_Status_t MPU_ReadGyroY_Raw(MPU_Handle_I2C_t *handle, int16_t *pData);
MPU_Status_t MPU_ReadGyroZ_Raw(MPU_Handle_I2C_t *handle, int16_t *pData);
MPU_Status_t MPU_ReadGyro_Raw(MPU_Handle_I2C_t *handle, int16_t *x, int16_t *y, int16_t *z);

/* FIFO operations */

MPU_Status_t MPU_SetFIFO(MPU_Handle_I2C_t *handle, uint8_t enable);
MPU_Status_t MPU_SetFIFO_Writing(MPU_Handle_I2C_t *handle);

MPU_Status_t MPU_DumpFIFO(MPU_Handle_I2C_t *handle, uint8_t buffer[], uint16_t *size);
MPU_Status_t MPU_GetFIFO_Size(MPU_Handle_I2C_t *handle, uint16_t *size);
MPU_Status_t MPU_ReadFIFO(MPU_Handle_I2C_t *handle, uint8_t buffer[]);
MPU_Status_t MPU_ProcessFIFO_Data(MPU_Handle_I2C_t *handle, uint8_t buffer[], MPU_Measurement_t *data);
MPU_Status_t MPU_ReadFIFO_Single(MPU_Handle_I2C_t *handle, uint8_t *value);
