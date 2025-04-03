#include "mpu6050_drv.h"

/* Service functions */

/**
 * \brief Converts MPU statuses into readable messages, i. e. for CLI.
 * \param[in] status Status.
 * \returns Message.
 */
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
    //CLI_Log(&ctx, __func__, "Initializing MPU6050.");
#ifdef USE_CLI
    CLI_AddCommand(&ctx, "getreg", &getreg_Handler, "Usage: getreg <address>, reads value stored in address");
    CLI_AddCommand(&ctx, "setreg", &setreg_Handler, "Usage: setreg <address> <value>, writes value to address");
#endif

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

    status = MPU_EnableFIFO(handle);
    printf("(Maybe) enabling FIFO: %s\n", MPU_Error2Str(status));

    return MPU_STATUS_OK;
}

/* High-level control functions */

MPU_Status_t MPU_SetClockSource(MPU_Handle_I2C_t *handle, MPU_ClockSource_t clock)
{
    MPU_Status_t status = MPU_WriteRegister_I2C(handle->hi2c, PWR_MGMT_1, \
        clock | (handle->temp_enable << 3) | (handle->cycle << 5));
    if (status != MPU_STATUS_OK) return status;
    handle->clock_source = clock;
    return MPU_STATUS_OK;
}

/**
 * \brief Enables FIFO writing for modules, selected in handle.
 * \param[in] handle MPU sensor handle.
 * \returns Operation status.
 */
MPU_Status_t MPU_EnableFIFO_Writing(MPU_Handle_I2C_t *handle)
{
    return MPU_WriteRegister_I2C(handle->hi2c, FIFO_EN, \
        (handle->accel_fifo << 3) | (handle->gyroz_fifo << 4) | \
        (handle->gyroy_fifo << 5) | (handle->gyrox_fifo << 6) | (handle->temp_fifo << 7));
}

/**
 * \brief Enables FIFO operations in general, to enable putting data into FIFO call 
 * MPU_Enable<Accel/Gyro>2FIFO.
 * \param[in] handle MPU sensor handle.
 * \returns Operation status.
 */
MPU_Status_t MPU_EnableFIFO(MPU_Handle_I2C_t *handle)
{
    return MPU_WriteRegister_I2C(handle->hi2c, USER_CTRL, 1 << 6);
}

/**
 * \brief Sets sensor's acceleration range to the one in handle's field.
 * \details Values are mapped that way:
 * | Value in field | Range |
 * | -------------- | ----- |
 * | 0              | +-2g  |
 * | 1              | +-4g  |
 * | 2              | +-8g  |
 * | 3              | +-16g |
 * \param[in] handle MPU sensor handle.
 * \param[in] range Accelerometer range, see table.
 * \returns Operation status.
 */
MPU_Status_t MPU_SetAccelRange(MPU_Handle_I2C_t *handle, uint8_t range)
{
    MPU_Status_t status = MPU_WriteRegister_I2C(handle->hi2c, ACCEL_CONFIG, \
        range << 3);
    if (status != MPU_STATUS_OK) return status;
    handle->acccel_range = range;
    return MPU_STATUS_OK;
}

/**
 * \brief Sets sensor's gyroscope range to the one in handle's field.
 * \details Values are mapped that way:
 * | Value in field | Range |
 * | -------------- | ----- |
 * | 0              | +-250 deg/sec  |
 * | 1              | +-500 deg/sec  |
 * | 2              | +-1000 deg/sec |
 * | 3              | +-2000 deg/sec |
 * \param[in] handle MPU sensor handle.
 * \param[in] range Gyroscope range, see table.
 * \returns Operation status.
 */
MPU_Status_t MPU_SetGyroRange(MPU_Handle_I2C_t *handle, uint8_t range)
{
    MPU_Status_t status = MPU_WriteRegister_I2C(handle->hi2c, GYRO_CONFIG, \
        range << 3);
    if (status != MPU_STATUS_OK) return status;
    handle->gyro_range = range;
    return MPU_STATUS_OK;
}

/* Reading accel data */

/**
 * \brief Reads raw acceleration across X axis.
 * \param[in] handle MPU sensor handle.
 * \param[out] pData Pointer to output value.
 * \returns Operation status. 
 */
MPU_Status_t MPU_ReadAccelX_Raw(MPU_Handle_I2C_t *handle, int16_t *pData)
{
    uint8_t buffer[2];
    MPU_Status_t status = MPU_ReadBytes_I2C(handle->hi2c, ACCEL_XOUT_H, 2, (uint8_t*)buffer);
    if (status != MPU_STATUS_OK) return status;
    *pData = ((int16_t)buffer[0] << 8) | buffer[1];
    return MPU_STATUS_OK;
}

/**
 * \brief Reads raw acceleration across Y axis.
 * \param[in] handle MPU sensor handle.
 * \param[out] pData Pointer to output value.
 * \returns Operation status. 
 */
MPU_Status_t MPU_ReadAccelY_Raw(MPU_Handle_I2C_t *handle, int16_t *pData)
{
    uint8_t buffer[2];
    MPU_Status_t status = MPU_ReadBytes_I2C(handle->hi2c, ACCEL_YOUT_H, 2, (uint8_t*)buffer);
    if (status != MPU_STATUS_OK) return status;
    *pData = ((int16_t)buffer[0] << 8) | buffer[1];
    return MPU_STATUS_OK;
}

/**
 * \brief Reads raw acceleration across Z axis.
 * \param[in] handle MPU sensor handle.
 * \param[out] pData Pointer to output value.
 * \returns Operation status. 
 */
MPU_Status_t MPU_ReadAccelZ_Raw(MPU_Handle_I2C_t *handle, int16_t *pData)
{
    uint8_t buffer[2];
    MPU_Status_t status = MPU_ReadBytes_I2C(handle->hi2c, ACCEL_ZOUT_H, 2, (uint8_t*)buffer);
    if (status != MPU_STATUS_OK) return status;
    *pData = ((int16_t)buffer[0] << 8) | buffer[1];
    return MPU_STATUS_OK;
}

/**
 * \brief Reads raw acceleration values across all axes.
 * \param[in] handle MPU sensor handle.
 * \param[out] x Pointer to X output value.
 * \param[out] y Pointer to Y output value.
 * \param[out] z Pointer to Z output value.
 * \returns Operation status.
 */
MPU_Status_t MPU_ReadAccel_Raw(MPU_Handle_I2C_t *handle, \
    int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t buffer[6];
    MPU_Status_t status = MPU_ReadBytes_I2C(handle->hi2c, ACCEL_XOUT_H, 6, buffer);
    if (status != MPU_STATUS_OK) return status;
    *x = ((int16_t)buffer[0] << 8) | buffer[1];
    *y = ((int16_t)buffer[2] << 8) | buffer[3];
    *z = ((int16_t)buffer[4] << 8) | buffer[5];
    return MPU_STATUS_OK;
}

/* Reading gyro data */

/**
 * \brief Reads raw gyroscope data across X axis.
 * \param[in] handle MPU sensor handle.
 * \param[out] pData Pointer to output value.
 * \returns Operation status. 
 */
MPU_Status_t MPU_ReadGyroX_Raw(MPU_Handle_I2C_t *handle, int16_t *pData)
{
    uint8_t buffer[2];
    MPU_Status_t status = MPU_ReadBytes_I2C(handle->hi2c, GYRO_XOUT_H, 2, (uint8_t*)buffer);
    if (status != MPU_STATUS_OK) return status;
    *pData = ((int16_t)buffer[0] << 8) | buffer[1];
    return MPU_STATUS_OK;
}

/**
 * \brief Reads raw gyroscope data across Y axis.
 * \param[in] handle MPU sensor handle.
 * \param[out] pData Pointer to output value.
 * \returns Operation status. 
 */
MPU_Status_t MPU_ReadGyroY_Raw(MPU_Handle_I2C_t *handle, int16_t *pData)
{
    uint8_t buffer[2];
    MPU_Status_t status = MPU_ReadBytes_I2C(handle->hi2c, GYRO_YOUT_H, 2, (uint8_t*)buffer);
    if (status != MPU_STATUS_OK) return status;
    *pData = ((int16_t)buffer[0] << 8) | buffer[1];
    return MPU_STATUS_OK;
}

/**
 * \brief Reads raw gyroscope data across Z axis.
 * \param[in] handle MPU sensor handle.
 * \param[out] pData Pointer to output value.
 * \returns Operation status. 
 */
MPU_Status_t MPU_ReadGyroZ_Raw(MPU_Handle_I2C_t *handle, int16_t *pData)
{
    uint8_t buffer[2];
    MPU_Status_t status = MPU_ReadBytes_I2C(handle->hi2c, GYRO_ZOUT_H, 2, (uint8_t*)buffer);
    if (status != MPU_STATUS_OK) return status;
    *pData = ((int16_t)buffer[0] << 8) | buffer[1];
    return MPU_STATUS_OK;
}

/**
 * \brief Reads raw gyroscope data values across all axes.
 * \param[in] handle MPU sensor handle.
 * \param[out] x Pointer to X output value.
 * \param[out] y Pointer to Y output value.
 * \param[out] z Pointer to Z output value.
 * \returns Operation status.
 */
MPU_Status_t MPU_ReadGyro_Raw(MPU_Handle_I2C_t *handle, \
    int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t buffer[6];
    MPU_Status_t status = MPU_ReadBytes_I2C(handle->hi2c, GYRO_XOUT_H, 6, buffer);
    if (status != MPU_STATUS_OK) return status;
    *x = ((int16_t)buffer[0] << 8) | buffer[1];
    *y = ((int16_t)buffer[2] << 8) | buffer[3];
    *z = ((int16_t)buffer[4] << 8) | buffer[5];
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

MPU_Status_t MPU_ReadBytes_I2C(I2C_HandleTypeDef *hi2c, MPU_Register_t register_addr, \
    uint32_t bytes, uint8_t *pData)
{
    if (HAL_I2C_Master_Transmit(hi2c, MPU6050_I2C_ADDRESS, \
        (uint8_t*)&register_addr, 1, MPU_TIMEOUT) != HAL_OK) {
           return MPU_STATUS_I2C_ERROR;
   }
   if (HAL_I2C_Master_Receive(hi2c, MPU6050_I2C_ADDRESS, \
       (uint8_t*)pData, bytes, MPU_TIMEOUT) != HAL_OK) {
           return MPU_STATUS_I2C_ERROR;
   }
   return MPU_STATUS_OK;
}