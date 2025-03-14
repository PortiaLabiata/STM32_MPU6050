#ifndef MAIN_H_
#define MAIN_H_

#include <stm32f1xx.h>
#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_rcc.h>
#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal_i2c.h>
#include <stm32f1xx_hal_uart.h>
#include <stm32f103x6.h>

#include <stdio.h>
#include <string.h>

#include "stm32f10x_it.h"
#include "configure.h"

#include "mpu6050_drv.h"
#include "mpu6050_const.h"
#include "mpu6050_typedef.h"
#include "cli.h"
#include "mpu6050_commands.h"

/* Magic numbers */


/* Types */

/* Global variables */

extern GPIO_InitTypeDef GPIO_InitStruc;
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;

/* User functions */



#endif