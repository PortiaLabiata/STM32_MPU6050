#include "main.h"

GPIO_InitTypeDef GPIO_InitStruc;
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    GPIO_Config();
    I2C_Config();
    UART1_Config();
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    CLI_AddCommand("getreg", getreg_Handler);
    CLI_AddCommand("setreg", setreg_Handler);
    uint8_t pData;
    char str[20];
    MPU6050_ReadRegister_I2C(&hi2c1, WHO_AM_I, (uint8_t*)&pData);
    sprintf(str, "%d\n", (int)pData);
    println(str);

    while (1) {
        CLI_ProcessCommand();
    }
}