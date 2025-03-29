#include "mpu6050_commands.h"

/* Command handlers */

CLI_Status_t setreg_Handler(int argc, char *argv[])
{
    if (argc < 2) return CLI_ERROR_ARG;
    uint8_t reg = strtol(argv[1], NULL, 16);
    uint8_t val = strtol(argv[2], NULL, 2);
    if (errno != 0) return CLI_ERROR_ARG;
    
    if (MPU_WriteRegister_I2C(&hi2c1, reg, val) != MPU_STATUS_OK) return CLI_ERROR;
    else return CLI_OK;
}

CLI_Status_t getreg_Handler(int argc, char *argv[])
{
    if (argc < 2) return CLI_ERROR_ARG;
    uint8_t reg = strtol(argv[1], NULL, 16);
    if (errno != 0) return CLI_ERROR_ARG;

    uint8_t value;
    if (MPU_ReadRegister_I2C(&hi2c1, reg, (uint8_t*)&value) != MPU_STATUS_OK) {
        return CLI_ERROR;
    }
    printf("%d register value is %d\r\n", (int)reg, value);
    return CLI_OK;
}