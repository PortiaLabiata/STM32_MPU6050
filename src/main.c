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
    MPU_WriteRegister_I2C(&hi2c1, PWR_MGMT_1, 0);
    MPU_WriteRegister_I2C(&hi2c1, ACCEL_CONFIG, 3 << 3);
    int16_t output = 0;
    uint8_t value = 0;

    /* MPU_Handle_I2C_t mpu;
    mpu.temp_enable = MPU_MOD_OFF;
    mpu.sample_rate_div = 0;
    mpu.hi2c = &hi2c1;
    mpu.gyro_range = 3;
    mpu.acccel_range = 3;
    mpu.clock_source = MPU_CLKS_INT;
    mpu.cycle = MPU_MOD_OFF;
    MPU_Init(&mpu); */
    while (1) {
        //CLI_RUN(&ctx);
        MPU_ReadRegister_I2C(&hi2c1, ACCEL_XOUT_L, &value);
        output += value;
        MPU_ReadRegister_I2C(&hi2c1, ACCEL_XOUT_H, &value);
        output += value << 8;
        printf("%d\n", output);
        output = 0;
    }
}