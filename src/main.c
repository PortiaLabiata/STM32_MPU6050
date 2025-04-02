#include "main.h"

GPIO_InitTypeDef GPIO_InitStruc;
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;

CLI_Context_t ctx;
MPU_Handle_I2C_t mpu;

void loop(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    GPIO_Config();
    I2C_Config();
    UART1_Config();
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    CLI_Status_t status = CLI_OK;
    CLI_Init(&ctx, &huart1);
    CLI_AddCommand(&ctx, "getreg", &getreg_Handler, "Usage: getreg <address>, reads value stored in address");
    CLI_AddCommand(&ctx, "setreg", &setreg_Handler, "Usage: setreg <address> <value>, writes value to address");
    CLI_Log(&ctx, __FILE__, "Peripherals configured, all ready.");

    mpu.temp_enable = MPU_MOD_OFF;
    mpu.sample_rate_div = 0;
    mpu.hi2c = &hi2c1;
    mpu.gyro_range = 0;
    mpu.acccel_range = 0;
    mpu.clock_source = MPU_CLKS_INT;
    mpu.cycle = MPU_MOD_OFF;
    MPU_Init(&mpu);
    
    while (1) {
        status = CLI_RUN(&ctx, loop);
        if (status != CLI_OK) {
            printf("Error: %s\n", CLI_Status2Str(status));
            CLI_Log(&ctx, __func__, "CLI Error!");
        }
    }
}

void loop(void)
{
    int16_t x, y, z;
    MPU_ReadGyro_Raw(&mpu, &x, &y, &z);
    printf("%d, %d, %d\n", x, y, z);
}