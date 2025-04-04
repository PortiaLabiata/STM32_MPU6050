#include "main.h"

GPIO_InitTypeDef GPIO_InitStruc;
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;

CLI_Context_t ctx;
MPU_Handle_I2C_t mpu;
uint8_t buffer[6];
uint16_t size;

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
    CLI_Log(&ctx, __FILE__, "Peripherals configured, all ready.");

    mpu.temp_enable = MPU_OFF;
    mpu.sample_rate_div = 0;
    mpu.hi2c = &hi2c1;
    mpu.gyro_range = 0;
    mpu.acccel_range = 3;
    mpu.clock_source = MPU_CLKS_INT;
    mpu.cycle = MPU_OFF;

    mpu.enable_fifo = MPU_ON;
    mpu.accel_fifo = MPU_ON;
    mpu.gyrox_fifo = MPU_OFF;
    mpu.gyroy_fifo = MPU_OFF;
    mpu.gyroz_fifo = MPU_OFF;
    mpu.temp_fifo = MPU_OFF;
    MPU_Init(&mpu);

    MPU_SetClockSource(&mpu, MPU_CLKS_PLLX);
    HAL_Delay(10);
    
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
    MPU_Status_t status = MPU_ReadFIFO(&mpu, buffer);
    if (status != MPU_STATUS_OK) {
        printf("%s\n", MPU_Error2Str(status));
        CLI_Log(&ctx, __func__, "Error(");
    }

    MPU_Measurement_t data;
    MPU_ProcessFIFO_Data(&mpu, buffer, &data);
    printf("%d, %d, %d\n", data.accel_x, data.accel_y, data.accel_z);
}