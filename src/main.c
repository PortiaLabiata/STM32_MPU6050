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

    CLI_Init(&huart1);
    CLI_AddCommand("getreg", &getreg_Handler, "Usage: getreg <address>, reads value stored in address");
    CLI_AddCommand("setreg", &setreg_Handler, "Usage: setreg <address> <value>, writes value to address");
    CLI_Log(__FILE__, "Peripherals configured, all ready.");

    while (1) {
        if (CLI_RUN() != CLI_OK) {
            CLI_Log(__func__, "CLI command runtime error.");
        }
    }
}