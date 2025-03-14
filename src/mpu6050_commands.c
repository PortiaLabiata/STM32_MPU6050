#include "mpu6050_commands.h"

/* Command handlers */

void setreg_Handler(char *argv[])
{
    uint8_t reg = strtol(argv[1], NULL, 16);
    uint8_t val = strtol(argv[2], NULL, 2);
    MPU6050_WriteRegister_I2C(&hi2c1, reg, val);
}

void getreg_Handler(char *argv[])
{
    uint8_t reg = strtol(argv[1], NULL, 16);
    uint8_t value;
    char output[100];
    MPU6050_ReadRegister_I2C(&hi2c1, reg, (uint8_t*)&value);
    sprintf(output, "%d register value is %d", (int)reg, value);
    println(output);
}