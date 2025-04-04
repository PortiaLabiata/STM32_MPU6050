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
        case MPU_STATUS_I2C_ERROR:
            return "MPU_STATUS_I2C_ERROR";
        case MPU_STATUS_ERROR:
            return "MPU_STATUS_ERROR";
        case MPU_STATUS_DNR_ERROR:
            return "MPU_STATUS_DNR_ERROR";
        default:
            return "Unknown status";
    }
}

/* Configuration functions */

MPU_Status_t MPU_Init(MPU_Handle_I2C_t *handle)
{
    if (handle == NULL) return MPU_STATUS_ERROR;
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

    status = MPU_SetAccelRange(handle, handle->acccel_range);
    printf("Setting accel range rate: %s\n", MPU_Error2Str(status));

    status = MPU_SetGyroRange(handle, handle->gyro_range);
    printf("Setting gyro range: %s\n", MPU_Error2Str(status));

    status = MPU_SetFIFO(handle, (uint8_t)handle->enable_fifo);
    printf("(Maybe) enabling FIFO: %s\n", MPU_Error2Str(status));

    status = MPU_SetFIFO_Writing(handle);
    printf("Enabling FIFO writing: %s\n", MPU_Error2Str(status));

    return MPU_STATUS_OK;
}

/* High-level control functions */

MPU_Status_t MPU_SetClockSource(MPU_Handle_I2C_t *handle, MPU_ClockSource_t clock)
{
    if (handle == NULL || (uint8_t)clock > (uint8_t)MPU_CLKS_DISABLE) return MPU_STATUS_ERROR;
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
MPU_Status_t MPU_SetFIFO_Writing(MPU_Handle_I2C_t *handle)
{
    return MPU_WriteRegister_I2C(handle->hi2c, FIFO_EN, \
        (handle->accel_fifo << 3) | (handle->gyroz_fifo << 4) | \
        (handle->gyroy_fifo << 5) | (handle->gyrox_fifo << 6) | (handle->temp_fifo << 7));
}

/**
 * \brief Enables FIFO operations in general, to enable putting data into FIFO call 
 * MPU_Enable<Accel/Gyro>2FIFO.
 * \param[in] handle MPU sensor handle.
 * \param[in] enable 1 is enable, 0 if disable.
 * \returns Operation status.
 */
MPU_Status_t MPU_SetFIFO(MPU_Handle_I2C_t *handle, uint8_t enable)
{
    return MPU_WriteRegister_I2C(handle->hi2c, USER_CTRL, enable << 6);
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
    if (handle == NULL || range > 3) return MPU_STATUS_ERROR;
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
    if (handle == NULL || range > 3) return MPU_STATUS_ERROR;

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
    if (handle == NULL || pData == NULL) return MPU_STATUS_ERROR;

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
    if (handle == NULL || pData == NULL) return MPU_STATUS_ERROR;

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
    if (handle == NULL || pData == NULL) return MPU_STATUS_ERROR;

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
    if (handle == NULL || x == NULL || y == NULL || z == NULL) return MPU_STATUS_ERROR;

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
    if (handle == NULL || pData == NULL) return MPU_STATUS_ERROR;

    if (handle == NULL || pData == NULL) return MPU_STATUS_ERROR;
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
    if (handle == NULL || pData == NULL) return MPU_STATUS_ERROR;

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
    if (handle == NULL || pData == NULL) return MPU_STATUS_ERROR;

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
    if (handle == NULL || x == NULL || y == NULL || z == NULL) return MPU_STATUS_ERROR;

    uint8_t buffer[6];
    MPU_Status_t status = MPU_ReadBytes_I2C(handle->hi2c, GYRO_XOUT_H, 6, buffer);
    if (status != MPU_STATUS_OK) return status;
    *x = ((int16_t)buffer[0] << 8) | buffer[1];
    *y = ((int16_t)buffer[2] << 8) | buffer[3];
    *z = ((int16_t)buffer[4] << 8) | buffer[5];
    return MPU_STATUS_OK;
}

/* FIFO operations */

/**
 * \brief Dumps the entirety of FIFO buffer.
 * \param[in] handle MPU sensor handle.
 * \param[out] buffer Buffer for incoming data.
 * \param[out] size Pointer to variable, where size of incoming data will be written
 *  (contents of FIFO_COUNT).
 * \returns Operation status.
 */
MPU_Status_t MPU_DumpFIFO(MPU_Handle_I2C_t *handle, uint8_t buffer[], uint16_t *size)
{
    if (handle == NULL || buffer == NULL || size == NULL) return MPU_STATUS_ERROR;
    MPU_Status_t status = MPU_GetFIFO_Size(handle, size);
    if (status != MPU_STATUS_OK) return status;

    for (int i = 0; i < *size; i++) {
        MPU_Status_t status = MPU_ReadRegister_I2C(handle->hi2c, FIFO_R_W, &buffer[i]);
        if (status != MPU_STATUS_OK) return status;
    }
    return MPU_STATUS_OK;
}

/**
 * \brief Reads current size of data in FIFO.
 * \param[in] handle MPU sensor handle.
 * \param[out] size Pointer to output variable.
 * \returns Operation status.
 */
MPU_Status_t MPU_GetFIFO_Size(MPU_Handle_I2C_t *handle, uint16_t *size)
{
    if (handle == NULL || size == NULL) return MPU_STATUS_ERROR;
    uint8_t _size[2];
    MPU_Status_t status = MPU_ReadBytes_I2C(handle->hi2c, FIFO_COUNT_H, 2, _size);
    if (status != MPU_STATUS_OK) return status;
    *size = (_size[0] << 8) | _size[1];
    return MPU_STATUS_OK;
}

/* Right now it doesn't work, but it's probably because the FIFO is overflowed very quickly */
/**
 * \brief Reads data from FIFO buffer. The size of data read depends on which FIFO writings
 * are enabled.
 * \param[in] handle MPU sensor handle.
 * \param[out] buffer Output buffer. It's size must be equal to accel_fifo + gyrox_fifo +
 * gyroy_fifo + gyroz_fifo + temp_fifo or more.
 */
MPU_Status_t MPU_ReadFIFO(MPU_Handle_I2C_t *handle, uint8_t buffer[])
{
    if (handle->enable_fifo !=  MPU_ON) return MPU_STATUS_ERROR;

    uint16_t len = (uint8_t)handle->accel_fifo * 6 + ((uint8_t)handle->gyrox_fifo + \
    (uint8_t)handle->gyroy_fifo + (uint8_t)handle->gyroz_fifo + \
    (uint8_t)handle->temp_fifo) * 2;

    uint16_t size = 0;
    MPU_Status_t status = MPU_GetFIFO_Size(handle, &size);
    if (status != MPU_STATUS_OK) return status;
    if (size < len) return MPU_STATUS_DNR_ERROR;
    
    for (int i = 0; i < len; i++) {
        status = MPU_ReadRegister_I2C(handle->hi2c, FIFO_R_W, &buffer[i]);
        if (status != MPU_STATUS_OK) return status;
    }
    return MPU_STATUS_OK;
}

/**
 * \brief Processes data, fetched from FIFO. Data must be already read by MPU_ReadFIFO.
 * \param[in] handle MPU sensor handle.
 * \param[in] buffer Incoming data.
 * \param[out] data Output structure.
 * \returns Operation status.
 */
MPU_Status_t MPU_ProcessFIFO_Data(MPU_Handle_I2C_t *handle, uint8_t buffer[], \
    MPU_Measurement_t *data)
{
    if (handle == NULL || buffer == NULL || data == NULL) return MPU_STATUS_ERROR;

    uint16_t of = 0;
    if (handle->accel_fifo == MPU_ON) {
        data->accel_x = BUFFER_SHIFT(of);
        data->accel_z = BUFFER_SHIFT(of);
        data->accel_z = BUFFER_SHIFT(of);
    }

    if (handle->temp_fifo == MPU_ON) {
        data->temp = BUFFER_SHIFT(of);
    }

    if (handle->gyrox_fifo == MPU_ON) {
        data->gyro_x = BUFFER_SHIFT(of);
    }
    if (handle->gyroy_fifo == MPU_ON) {
        data->gyro_y = BUFFER_SHIFT(of);
    }
    if (handle->gyroz_fifo == MPU_ON) {
        data->gyro_z = BUFFER_SHIFT(of);
    }

    return MPU_STATUS_OK;
}

/**
 * \brief Reads one FIFO entry. There is no check for FIFO size, because it is too long.
 * \param[in] handle MPU sensor handle.
 * \param[out] value Pointer to output variable.
 * \returns Operation status.
 */
MPU_Status_t MPU_ReadFIFO_Single(MPU_Handle_I2C_t *handle, uint8_t *value)
{
    return MPU_ReadRegister_I2C(handle->hi2c, FIFO_R_W, value);
}

/* Low-level I2C functions. Error handling not added for better performance? */
/* Is it a bad idea? */

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