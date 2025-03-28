#ifndef CONF_H_
#define CONF_H_

#include "main.h"

/* Basic configuration */

void SystemClock_Config(void);
void GPIO_Config(void);
void I2C_Config(void);
void UART1_Config(void);

/* Msp configuration */

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c);
void HAL_UART_MspInit(UART_HandleTypeDef* huart);
void HAL_MspInit(void);

#endif