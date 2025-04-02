#include "mpu6050_drv.h"

/* Service functions */

char *MPU_Error2Str(MPU_Status_t status)
{
    switch (status) {
        case MPU_STATUS_OK:
            return "MPU_STATUS_OK";
            break;
        case MPU_STATUS_I2C_ERROR:
            return "MPU_STATUS_I2C_ERROR";
            break;
        case MPU_STATUS_ERROR:
            return "MPU_STATUS_ERROR";
            break;
        default:
            return "Unknown status";
            break; 
    }
}

/* Configuration functions */

MPU_Status_t MPU_Init(MPU_Handle_I2C_t *handle)
{
    CLI_Log(&ctx, __func__, "Initializing MPU6050.");
    uint8_t value = handle->clock_source | \
                     (handle->temp_enable << 3) | \
                     (handle->cycle << 5);
    MPU_Status_t status = MPU_WriteRegister_I2C(handle->hi2c, PWR_MGMT_1, value);
    printf("Setting power options: %s\n", MPU_Error2Str(status));

    value = handle->sample_rate_div;
    status = MPU_WriteRegister_I2C(handle->hi2c, SMPLRT_DIV, value);
    printf("Setting sample rate: %s\n", MPU_Error2Str(status));

    status = MPU_SetAccelRange(handle);
    printf("Setting accel range rate: %s\n", MPU_Error2Str(status));

    status = MPU_SetGyroRange(handle);
    printf("Setting gyro range: %s\n", MPU_Error2Str(status));

    return MPU_STATUS_OK;
}

/* High-level control functions */

MPU_Status_t MPU_Start(MPU_Handle_I2C_t *handle)
{
    return MPU_STATUS_OK;
}

MPU_Status_t MPU_EnableFIFO(MPU_Handle_I2C_t *handle)
{
    return MPU_STATUS_OK;
}

MPU_Status_t MPU_SetAccelRange(MPU_Handle_I2C_t *handle)
{
    return MPU_WriteRegister_I2C(handle->hi2c, ACCEL_CONFIG, handle->acccel_range << 3);
}

MPU_Status_t MPU_SetGyroRange(MPU_Handle_I2C_t *handle)
{
    return MPU_WriteRegister_I2C(handle->hi2c, GYRO_CONFIG, handle->gyro_range << 3);
}

/* Low-level I2C functions */

MPU_Status_t MPU_WriteRegister_I2C(I2C_HandleTypeDef *hi2c, \
    MPU_Register_t register_addr, uint8_t value)
{
    uint8_t pData[2] = {(uint8_t)register_addr, value};
    if (HAL_I2C_Master_Transmit(hi2c, MPU6050_I2C_ADDRESS, \
         (uint8_t*)pData, 2, MPU_TIMEOUT) !=  HAL_OK) {
        return MPU_STATUS_I2C_ERROR;
    } else {
        return MPU_STATUS_OK;
    }
}

MPU_Status_t MPU_ReadRegister_I2C(I2C_HandleTypeDef *hi2c, MPU_Register_t register_addr, \
    uint8_t *pData)
{
    if (HAL_I2C_Master_Transmit(hi2c, MPU6050_I2C_ADDRESS, \
         (uint8_t*)&register_addr, 1, MPU_TIMEOUT) != HAL_OK) {
            return MPU_STATUS_I2C_ERROR;
    }
    if (HAL_I2C_Master_Receive(hi2c, MPU6050_I2C_ADDRESS, \
        (uint8_t*)pData, 1, MPU_TIMEOUT) != HAL_OK) {
            return MPU_STATUS_I2C_ERROR;
    }
    return MPU_STATUS_OK;
}