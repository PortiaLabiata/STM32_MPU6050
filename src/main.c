#include "main.h"

GPIO_InitTypeDef GPIO_InitStruc;
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;

CLI_Context_t ctx;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    GPIO_Config();
    I2C_Config();
    UART1_Config();
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    CLI_Init(&ctx, &huart1);
    CLI_AddCommand(&ctx, "getreg", &getreg_Handler, "Usage: getreg <address>, reads value stored in address");
    CLI_AddCommand(&ctx, "setreg", &setreg_Handler, "Usage: setreg <address> <value>, writes value to address");
    CLI_Log(&ctx, __FILE__, "Peripherals configured, all ready.");

    CLI_Status_t status = CLI_OK;
    while (1) {
        if ((status = CLI_RUN(&ctx)) != CLI_OK) {
            CLI_Log(&ctx, __func__, CLI_Status2Str(status));
        }
    }
}