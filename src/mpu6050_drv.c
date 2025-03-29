#include "mpu6050_drv.h"

/* Configuration functions */

MPU_Status_t MPU_Init(MPU_Handle_I2C_t *handle)
{
    CLI_Log(&ctx, __func__, "Initializing MPU6050.");
    uint8_t value = handle->clock_source | \
                     (handle->temp_enable << 3) | \
                     (handle->cycle << 5);
    MPU_WriteRegister_I2C(handle, PWR_MGMT_1, value);
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