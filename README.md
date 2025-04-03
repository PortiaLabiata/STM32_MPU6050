# A simple driver for MPU6050 sensor

## Disclaimer

This driver is untested, likely buggy and inefficient, so don't use it for any critical applications, where human lives, health or property might be at stake.

## Usage

### How to include it in your project

To use this driver, simply copy it's files to their corresponding directories in your project and use

    #include "mpu6050_drv.h"

To use debug CLI, add `-D USE_CLI` to your build flags and include `cli.h`. Details on the interface can be found in it's repo: [h](https://github.com/PortiaLabiata/STM32_CLI.git).

### Initialization

To initialize the sensor, `MPU_Handle_t` structure is used. It's fields are:

    typedef struct {
        I2C_HandleTypeDef *hi2c;            // HAL I2C structure
        MPU_ClockSource_t clock_source;     // Clock source one of MPU_ClockSource_t, see below.
        MPU_ModuleState_t temp_enable;      // Enable temperature sensor, see below.
        MPU_ModuleState_t cycle;            // Enable sleep cycling.
        uint8_t sample_rate_div;            // Sample rate divider.

        uint8_t acccel_range;               // Accelerometer full range, see below.
        uint8_t gyro_range;                 // Gyroscope full range.

        MPU_ModuleState_t accel_fifo;       // Put accelerometer data into FIFO buffer.
        MPU_ModuleState_t gyrox_fifo;       // Put gyroscope X axis data into FIFO.
        MPU_ModuleState_t gyroy_fifo;       // Put gyroscope Y axis data into FIFO.
        MPU_ModuleState_t gyroz_fifo;       // Put gyroscope Z axis data into FIFO.
        MPU_ModuleState_t temp_fifo;        // Put temperature data into FIFO.
        MPU_ModuleState_t enable_fifo;      // Enable FIFO operations.
    } MPU_Handle_I2C_t;

### Clock sources

Clock sources are as follows:

| Value            | Explanation                              |
|------------------|------------------------------------------|
| MPU_CLKS_INT     | Internal 8MHz clock                      |
| MPU_CLKS_PLLX    | Accelerometer X axis oscillator with PLL |
| MPU_CLKS_PLLY    | Accelerometer Y axis oscillator with PLL |
| MPU_CLKS_PLLZ    | Accelerometer Z axis oscillator with PLL |
| MPU_CLKS_EXT32K  | External 32.768kHz clock                 |
| MPU_CLKS_EXT19M  | External 19MHz clock                     |
| MPU_CLKS_DISABLE | Disable clock                            |

Obviously, if accelerometer is disabled, using clock signal from it is impossible, so sensor will automatically switch to internal 8MHz clock. More detailed information can be found in the datasheet. To set clock source from your code, call `MPU_SetClockSource(MPU_Handle_I2C_t *handle, MPU_ClockSource_t clock)`.

### Data ranges

It is possible to configure full range of accelerometer and gyroscope, obviously it will affect their sensitivity. For the sake of convenience, here are the possible values for this field:

| Value | Range    |
|-------|----------|
| 0     | $\pm2g$  |
| 1     | $\pm4g$  |
| 2     | $\pm8g$  |
| 3     | $\pm16g$ |

| Value | Range         |
|-------|---------------|
| 0     | $\pm250^o/s$  |
| 1     | $\pm500^o/s$  |
| 2     | $\pm1000^o/s$ |
| 3     | $\pm2000^o/s$ |

To set gyroscope or accelerometer range from your code, call `MPU_Status_t MPU_SetGyroRange(MPU_Handle_I2C_t *handle, uint8_t range)` or `MPU_Status_t MPU_SetAccelRange(MPU_Handle_I2C_t *handle, uint8_t range)`.

### Performing measurements

To read accelerometer data, use one of the following functions:
1. `MPU_Status_t MPU_ReadAccel<X/Y/Z>_Raw(MPU_Handle_I2C_t *handle, int16_t *pData)` - reads raw acceleration across X/Y/Z axis.
2. `MPU_Status_t MPU_ReadAccel_Raw(MPU_Handle_I2C_t *handle, int16_t *x, int16_t *y, int16_t *z)` - reads raw values of accelerations across all three axes.

To read gyroscope data, similar functions are used.