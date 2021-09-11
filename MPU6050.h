#ifndef MPU6050_I2C_DRIVER_H
#define MPU6050_I2C_DRIVER_H

#include <math.h>
#include <stdbool.h>
#include "Action_HAL.h"

/** Device address
 *
 * ASEL = 0 -> 0xD0 or 0x68
 * ASEL = 1 -> 0xD1 ox 0x69
 *
 * */
#define MPU6050_I2C_ADDR 0x68
// Registers

#define MPU6050_REG_SELF_TEST_X 0x0D
#define MPU6050_REG_SELF_TEST_Y 0x0E
#define MPU6050_REG_SELF_TEST_Z 0x0F
#define MPU6050_REG_SELF_TEST_A 0x10
#define MPU6050_REG_SMPLRT_DIV 0x19
#define MPU6050_REG_CONFIG 0x1A
#define MPU6050_REG_GYRO_CONFIG 0x1B
#define MPU6050_REG_ACCEL_CONFIG 0x1C
#define MPU6050_REG_FIFO_EN 0x23
#define MPU6050_REG_I2C_MST_CTRL 0x24
#define MPU6050_REG_I2C_SLV0_ADDR 0x25
#define MPU6050_REG_I2C_SLV0_REG 0x26
#define MPU6050_REG_I2C_SLV0_CTRL 0x27
#define MPU6050_REG_I2C_SLV1_ADDR 0x28
#define MPU6050_REG_I2C_SLV1_REG 0x29
#define MPU6050_REG_I2C_SLV1_CTRL 0x2A
#define MPU6050_REG_I2C_SLV2_ADDR 0x2B
#define MPU6050_REG_I2C_SLV2_REG 0x2C
#define MPU6050_REG_I2C_SLV2_CTRL 0x2D
#define MPU6050_REG_I2C_SLV3_ADDR 0x2E
#define MPU6050_REG_I2C_SLV3_REG 0x2F
#define MPU6050_REG_I2C_SLV3_CTRL 0x30
#define MPU6050_REG_I2C_SLV4_ADDR 0x31
#define MPU6050_REG_I2C_SLV4_REG 0x32
#define MPU6050_REG_I2C_SLV4_DO 0x33
#define MPU6050_REG_I2C_SLV4_CTRL 0x34
#define MPU6050_REG_I2C_SLV4_DI 0x35
#define MPU6050_REG_I2C_MST_STATUS 0x36
#define MPU6050_REG_INT_PIN_CFG 0x37
#define MPU6050_REG_INT_ENABLE 0x38
#define MPU6050_REG_INT_STATUS 0x3A
#define MPU6050_REG_ACCEL_XOUT_H 0x3B
#define MPU6050_REG_ACCEL_XOUT_L 0x3C
#define MPU6050_REG_ACCEL_YOUT_H 0x3D
#define MPU6050_REG_ACCEL_YOUT_L 0x3E
#define MPU6050_REG_ACCEL_ZOUT_H 0x3F
#define MPU6050_REG_ACCEL_ZOUT_L 0x40
#define MPU6050_REG_TEMP_OUT_H 0x41
#define MPU6050_REG_TEMP_OUT_L 0x42
#define MPU6050_REG_GYRO_XOUT_H 0x43
#define MPU6050_REG_GYRO_XOUT_L 0x44
#define MPU6050_REG_GYRO_YOUT_H 0x45
#define MPU6050_REG_GYRO_YOUT_L 0x46
#define MPU6050_REG_GYRO_ZOUT_H 0x47
#define MPU6050_REG_GYRO_ZOUT_L 0x48
#define MPU6050_REG_EXT_SENS_DATA_00 0x49
#define MPU6050_REG_EXT_SENS_DATA_01 0x4A
#define MPU6050_REG_EXT_SENS_DATA_02 0x4B
#define MPU6050_REG_EXT_SENS_DATA_03 0x4C
#define MPU6050_REG_EXT_SENS_DATA_04 0x4D
#define MPU6050_REG_EXT_SENS_DATA_05 0x4E
#define MPU6050_REG_EXT_SENS_DATA_06 0x4F
#define MPU6050_REG_EXT_SENS_DATA_07 0x50
#define MPU6050_REG_EXT_SENS_DATA_08 0x51
#define MPU6050_REG_EXT_SENS_DATA_09 0x52
#define MPU6050_REG_EXT_SENS_DATA_10 0x53
#define MPU6050_REG_EXT_SENS_DATA_11 0x54
#define MPU6050_REG_EXT_SENS_DATA_12 0x55
#define MPU6050_REG_EXT_SENS_DATA_13 0x56
#define MPU6050_REG_EXT_SENS_DATA_14 0x57
#define MPU6050_REG_EXT_SENS_DATA_15 0x58
#define MPU6050_REG_EXT_SENS_DATA_16 0x59
#define MPU6050_REG_EXT_SENS_DATA_17 0x5A
#define MPU6050_REG_EXT_SENS_DATA_18 0x5B
#define MPU6050_REG_EXT_SENS_DATA_19 0x5C
#define MPU6050_REG_EXT_SENS_DATA_20 0x5D
#define MPU6050_REG_EXT_SENS_DATA_21 0x5E
#define MPU6050_REG_EXT_SENS_DATA_22 0x5F
#define MPU6050_REG_EXT_SENS_DATA_23 0x60
#define MPU6050_REG_I2C_SLV0_DO 0x63
#define MPU6050_REG_I2C_SLV1_DO 0x64
#define MPU6050_REG_I2C_SLV2_DO 0x65
#define MPU6050_REG_I2C_SLV3_DO 0x66
#define MPU6050_REG_I2C_MST_DELAY_CTRL 0x67
#define MPU6050_REG_SIGNAL_PATH_RESET 0x68
#define MPU6050_REG_USER_CTRL 0x6A
#define MPU6050_REG_PWR_MGMT_1 0x6B
#define MPU6050_REG_PWR_MGMT_2 0x6C
#define MPU6050_REG_FIFO_COUNTH 0x72
#define MPU6050_REG_FIFO_COUNTL 0x73
#define MPU6050_REG_FIFO_R_W 0x74
#define MPU6050_REG_WHO_AM_I 0x75

// Settings //
// Reset values
#define MPU6050_PWR_MGMT_1_RESET 0x40
#define MPU6050_WHO_AM_I_RESET 0x68

// Options

/* Frame Synchronization */
#define MPU6050_CONFIG_FSYNC_DISABLED 0x00 << 3
#define MPU6050_CONFIG_FSYNC_TEMP_L 0x01 << 3
#define MPU6050_CONFIG_FSYNC_GYRO_XL 0x02 << 3
#define MPU6050_CONFIG_FSYNC_GYRO_YL 0x03 << 3
#define MPU6050_CONFIG_FSYNC_GYRO_ZL 0x04 << 3
#define MPU6050_CONFIG_FSYNC_ACCEL_XL 0x05 << 3
#define MPU6050_CONFIG_FSYNC_ACCEL_YL 0x06 << 3
#define MPU6050_CONFIG_FSYNC_ACCEL_ZL 0x07 << 3

/* Accelerometer and Gyroscope Bandwidth */
#define MPU6050_CONFIG_DLPF_ACCEL_260_GYRO_256 0x00
#define MPU6050_CONFIG_DLPF_ACCEL_184_GYRO_188 0x01
#define MPU6050_CONFIG_DLPF_ACCEL_94_GYRO_98 0x02
#define MPU6050_CONFIG_DLPF_ACCEL_44_GYRO_42 0x03
#define MPU6050_CONFIG_DLPF_ACCEL_21_GYRO_20 0x04
#define MPU6050_CONFIG_DLPF_ACCEL_10_GYRO_10 0x05
#define MPU6050_CONFIG_DLPF_ACCEL_5_GYRO_5 0x06

/* Gyroscope Configuration values (pg 14)*/
#define MPU6050_GYRO_CONFIG_250DPS                                             \
  0x00 << 3 // Configure the gyroscope to work at 250 degrees per second
#define MPU6050_GYRO_CONFIG_500DPS                                             \
  0x01 << 3 // Configure the gyroscope to work at 500 degrees per second
#define MPU6050_GYRO_CONFIG_1000DPS                                            \
  0x02 << 3 // Configure the gyroscope to work at 1000 degrees per second
#define MPU6050_GYRO_CONFIG_2000DPS                                            \
  0x03 << 3 // Configure the gyroscope to work at 2000 degrees per second
#define MPU6050_GYRO_CONFIG_X_SELF_TEST                                        \
  0x80 // Turn on the X axis gyroscope self test
#define MPU6050_GYRO_CONFIG_Y_SELF_TEST                                        \
  0x40 // Turn on the Y axis gyroscope self test
#define MPU6050_GYRO_CONFIG_Z_SELF_TEST                                        \
  0x20 // Turn on the Z axis gyroscope self test
#define MPU6050_GYRO_CONFIG_ALL_SELF_TEST                                      \
  0xE0 // Turn on all the gyroscope axis' self test

/* Accelerometer Configuration values (pg 15)*/
#define MPU6050_ACCEL_CONFIG_2G                                                \
  0x00 << 3 // Configure the accelerometer to work at 250 degrees per second
#define MPU6050_ACCEL_CONFIG_4G                                                \
  0x01 << 3 // Configure the accelerometer to work at 500 degrees per second
#define MPU6050_ACCEL_CONFIG_8G                                                \
  0x02 << 3 // Configure the accelerometer to work at 1000 degrees per second
#define MPU6050_ACCEL_CONFIG_16G                                               \
  0x03 << 3 // Configure the accelerometer to work at 2000 degrees per second
#define MPU6050_ACCEL_CONFIG_X_SELF_TEST                                       \
  0x80 // Turn on the X axis accelerometer self test
#define MPU6050_ACCEL_CONFIG_Y_SELF_TEST                                       \
  0x40 // Turn on the Y axis accelerometer self test
#define MPU6050_ACCEL_CONFIG_Z_SELF_TEST                                       \
  0x20 // Turn on the Z axis accelerometer self test
#define MPU6050_ACCEL_CONFIG_ALL_SELF_TEST                                     \
  0xE0 // Turn on all the accelerometer axis' self test

/* FIFO Enable */
#define MPU6050_FIFO_EN_TEMP_FIFO_EN 0x01 << 7
#define MPU6050_FIFO_EN_XG_FIFO_EN 0x01 << 6
#define MPU6050_FIFO_EN_YG_FIFO_EN 0x01 << 5
#define MPU6050_FIFO_EN_ZG_FIFO_EN 0x01 << 4
#define MPU6050_FIFO_EN_ACCEL_FIFO_EN 0x01 << 3
#define MPU6050_FIFO_EN_SLV2_FIFO_EN 0x01 << 2
#define MPU6050_FIFO_EN_SLV1_FIFO_EN 0x01 << 1
#define MPU6050_FIFO_EN_SLV0_FIFO_EN 0x01 << 0
#define MPU6050_FIFO_EN_ALL_SENS 0xF4
#define MPU6050_FIFO_EN_ALL_SLV 0x07

/* Define interrupt settings (page 26) */
#define MPU6050_INT_PIN_CFG_INT_LEVEL                                          \
  0x01 << 7 // Set interupt pin to active low (otherwise it is active high)
#define MPU6050_INT_PIN_CFG_INT_OPEN 0x01 << 6
// set interupt pin to open drain (otherwise it is push-pull)
#define MPU6050_INT_PIN_CFG_LATCH_INT_EN                                       \
  0x01 << 5 // Interupt pin will be held high/low till cleared (otherwise it
            // will pulse for 50us)
#define MPU6050_INT_PIN_CFG_INT_RD_CLEAR                                       \
  0x01 << 4 // Interrupt Status bits cleared by any read (otherwise cleared by
            // reading the INT_STATUS register (MPU6050_REG_INT_STATUS))
#define MPU6050_INT_PIN_CFG_FSYNC_INT_LEVEL                                    \
  0x01 << 3 // FSYNC interupt (if used) will be active low (otherwise it is
            // active high)
#define MPU6050_INT_PIN_CFG_FSYNC_INT_EN                                       \
  0x01 << 2 // FSYNC interupt pin will be used (otherwise it will not be used)
#define MPU6050_INT_PIN_CFG_I2C_BYPASS_EN                                      \
  0x01 << 1 // Host will not be able to access the auxiliary I2C bus despite the
            // state of the I2C_MST_EN bit (MCP6050_REG_I2C_MST_EN).

#define MPU6050_FIFO_OFLOW                                                     \
  0x01 << 4 // Enables an interupt to go off at FIFO overflows (otherwise no
            // warning is given) or Reads the status of the interrupt
#define MPU6050_I2C_MST                                                        \
  0x01 << 3 // Enables I2C Master interrupt sources to generate an interupt
            // (otherwise they can not) or Reads the status of the interrupt
#define MPU6050_DATA_RDY                                                       \
  0x01 << 0 // Enables the Data Ready interrupt  or Reads the status of the
            // interrupt

#define MPU6050_PWR_MGMT_1_DEVICE_RESET                                        \
  0x80 // Resets the device if set and clears when done. Sets all registers to
       // default values
#define MPU6050_PWR_MGMT_1_DEVICE_SLEEP                                        \
  0x40 // Puts the device in sleep mode when set
#define MPU6050_PWR_MGMT_1_DEVICE_CYCLE                                        \
  0x20 // Cycles between sleep and awake mode to sample data at a rate given by
       // MPU6050_PWR_MGMT_2_LP_WAKE_CTRL
#define MPU6050_PWR_MGMT_1_DEVICE_TEMP_DIS                                     \
  0x04 // When set, the temperature sensor is disabled
#define MPU6050_PWR_MGMT_1_DEVICE_CLKSEL_8MHZ                                  \
  0x00 // Sets clock source to be the internal 8MHz oscillator
#define MPU6050_PWR_MGMT_1_DEVICE_CLKSEL_PLL_X_GYRO                            \
  0x01 // Sets clock source to be a PLL with the X-axis gyroscope
#define MPU6050_PWR_MGMT_1_DEVICE_CLKSEL_PLL_Y_GYRO                            \
  0x02 // Sets clock source to be a PLL with the Y-axis gyroscope
#define MPU6050_PWR_MGMT_1_DEVICE_CLKSEL_PLL_Z_GYRO                            \
  0x03 // Sets clock source to be a PLL with the Z-axis gyroscope
#define MPU6050_PWR_MGMT_1_DEVICE_CLKSEL_PLL_32KHZ                             \
  0x04 // Sets clock source to be a PLL with an external 32.768kHz reference
#define MPU6050_PWR_MGMT_1_DEVICE_CLKSEL_PLL_19MHZ                             \
  0x05 // Sets clock source to be a PLL with an external 19.2MHz reference
#define MPU6050_PWR_MGMT_1_DEVICE_CLKSEL_STOP                                  \
  0x07 // Stops the clock and keeps the timing generator in reset


#define PI 3.14159265

typedef struct {
  /*I2C handle*/
  int *i2cHandle;
  // I2C_HandleTypeDef *i2cHandle;

  float accel_scale;
  float gyro_scale;

  /* Interrupts */
  __uint8_t interrupts;

  bool interruptable;

  /* Acceleration Data (m/s^2) [X,Y,Z] */
  __int16_t acc[3];

  /* Gyroscope Data (rad/s) [X,Y,Z] */
  __int16_t rad[3];

  /* Temperature Data (C) */
  __int16_t temp_C;

  /* Full data packet */
  __int16_t data[7];
  __int16_t prev_data[7];
  __int16_t calibration[7];

  __int16_t acc_pitch, acc_roll, acc_yaw;
  __int16_t gyr_pitch, gyr_roll, gyr_yaw;
  __int16_t pitch, roll;
} MPU6050;

HAL_StatusTypeDef MPU6050_Init(MPU6050 *device, int *handle);
// HAL_StatusTypeDef MPU6050_Init(MPU6050 *device, I2C_HandleTypeDef *handle);

HAL_StatusTypeDef MPU6050_Set_Sample_Rate(MPU6050 *device, __uint8_t rate);
HAL_StatusTypeDef MPU6050_Set_Filter_Bandwidth(MPU6050 *device,
                                               __uint8_t bandwidth);
HAL_StatusTypeDef MPU6050_Set_Gyroscope_Range(MPU6050 *device, __uint8_t range);
HAL_StatusTypeDef MPU6050_Set_Accelerometer_Range(MPU6050 *device,
                                                  __uint8_t range);

HAL_StatusTypeDef MPU6050_Calibrate_Device(MPU6050 *device);
bool MPU6050_Is_Data_Ready(MPU6050 *device);
HAL_StatusTypeDef MPU6050_Enable_Interrupt(MPU6050 *device,
                                           __uint8_t interrupt);

HAL_StatusTypeDef MPU6050_Calculate_Pitch_Roll(MPU6050 *device);

/* Conversion read methods*/
HAL_StatusTypeDef MPU6050_Read_Temperature(MPU6050 *device);
HAL_StatusTypeDef MPU6050_Read_Acceleration(MPU6050 *device);
HAL_StatusTypeDef MPU6050_Read_Gyroscope(MPU6050 *device);
HAL_StatusTypeDef MPU6050_Read_All_Sensors(MPU6050 *device);
HAL_StatusTypeDef MPU6050_Enable_Sensor_FIFO(MPU6050 *device);

/* General methods to interact with the device */
HAL_StatusTypeDef MPU6050_Run_Self_Test(MPU6050 *device);

/* Device read methods */
HAL_StatusTypeDef MPU6050_Read_Interrupts(MPU6050 *device);

HAL_StatusTypeDef MPU6050_Read_Register(MPU6050 *device, __uint8_t reg,
                                        __uint8_t *data);

HAL_StatusTypeDef MPU6050_Read_Registers(MPU6050 *device, __uint8_t reg,
//                                          __uint8_t *data, __uint8_t length);

HAL_StatusTypeDef MPU6050_Write_Register(MPU6050 *device, __uint8_t reg,
                                         __uint8_t *data);

#endif
