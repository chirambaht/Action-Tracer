#include "MPU6050.h"


HAL_StatusTypeDef MPU6050_Init(MPU6050 *device, int *handle){
// HAL_StatusTypeDef MPU6050_Init(MPU6050 *device, I2C_HandleTypeDef *handle) {
  /**
   * Things to note:
   * 1. The accelerometer samples at 1kHz at most (expect duplicates)
   * 2. Why not read from the FIFO? This data is totally synced with all the
   * sensors.
   *
   */

  /* This is a byte with the data that's been read from a register*/
  __uint8_t reg_data;

  /* Setting up struct parameters*/
  device->i2cHandle = handle;

  device->interrupts = 0;

  device->acc[0] = 0;
  device->acc[1] = 0;
  device->acc[2] = 0;

  device->rad[0] = 0;
  device->rad[1] = 0;
  device->rad[2] = 0;

  device->temp_C = 0.0f;

  device->gyro_scale = 131;
  device->accel_scale = 16384;

	device->acc_pitch = 0;
	device->acc_roll = 0;
	device->acc_yaw= 0;


	device->gyr_pitch = 0;
	device->gyr_roll = 0;
	device->gyr_yaw = 0;

  for (__uint8_t i = 0; i < 7; i++){
	  device->calibration[i] = 0;
	  device->data[i] = 0;
  }

  HAL_StatusTypeDef status;
  /* Check if the device is ready */
  status = HAL_I2C_IsDeviceReady(device->i2cHandle, MPU6050_I2C_ADDR, 1, 1000);
  if (status != HAL_OK) {
    return status;
  }

  /* Check device address */

  if (MPU6050_Read_Register(device, MPU6050_REG_WHO_AM_I, &reg_data) !=
      HAL_OK) {
    return HAL_ERROR;
  }

  if (reg_data != MPU6050_WHO_AM_I_RESET) {
    return HAL_ERROR;
  }

  /* Set device's filter bandwidth */
  MPU6050_Set_Filter_Bandwidth(device, MPU6050_CONFIG_DLPF_ACCEL_260_GYRO_256);

  /* Set devices accelerometer range */
  MPU6050_Set_Accelerometer_Range(device, MPU6050_ACCEL_CONFIG_8G);

  /* Set devices gyroscope range */
  MPU6050_Set_Gyroscope_Range(device, MPU6050_GYRO_CONFIG_2000DPS);

  reg_data = MPU6050_PWR_MGMT_1_DEVICE_RESET;
  /* Set the device to reset itself and it's registers */
  MPU6050_Write_Register(device, MPU6050_REG_PWR_MGMT_1, &reg_data);
  MPU6050_Read_Register(device, MPU6050_REG_PWR_MGMT_1, &reg_data);

  HAL_Delay(100);

  /**
   * Wake up the device and set the clock source
   * Syntax is WAKE_UP(!SLEEP) | SET_CLOCK
   * */
  //	reg_data = (!MPU6050_PWR_MGMT_1_DEVICE_SLEEP) |
  // MPU6050_PWR_MGMT_1_DEVICE_CLKSEL_PLL_32KHZ;
  reg_data = 0;
  MPU6050_Write_Register(device, MPU6050_REG_PWR_MGMT_1, &reg_data);

  MPU6050_Calibrate_Device(device);

  return HAL_OK;
}

HAL_StatusTypeDef MPU6050_Set_Gyroscope_Range(MPU6050 *device,
                                              __uint8_t range) {

  __uint8_t data;

  MPU6050_Read_Register(device, MPU6050_REG_GYRO_CONFIG, &data);

  data = (data & 0xE7) | range; // Clear FS_SEL bits and assign new value

  switch (range) {
  case MPU6050_GYRO_CONFIG_2000DPS:
    device->gyro_scale = 16.4;
    break;
  case MPU6050_GYRO_CONFIG_1000DPS:
    device->gyro_scale = 32.8;
    break;
  case MPU6050_GYRO_CONFIG_500DPS:
    device->gyro_scale = 65.5;
    break;
  case MPU6050_GYRO_CONFIG_250DPS:
    device->gyro_scale = 131;
    break;
  default:
    device->gyro_scale = 131;
    break;
  }

  return MPU6050_Write_Register(device, MPU6050_REG_GYRO_CONFIG, &data);
}

HAL_StatusTypeDef MPU6050_Set_Accelerometer_Range(MPU6050 *device,
                                                  __uint8_t range) {
  __uint8_t data;

  MPU6050_Read_Register(device, MPU6050_REG_ACCEL_CONFIG, &data);

  data = (data & 0xE7) | range; // Clear AFS_SEL bits and assign new value

  switch (range) {
  case MPU6050_ACCEL_CONFIG_16G:
    device->accel_scale = 2048;
    break;
  case MPU6050_ACCEL_CONFIG_8G:
    device->accel_scale = 4096;
    break;
  case MPU6050_ACCEL_CONFIG_4G:
    device->accel_scale = 8192;
    break;
  case MPU6050_ACCEL_CONFIG_2G:
    device->accel_scale = 16384;
    break;
  default:
    device->accel_scale = 16384;
    break;
  }
  return MPU6050_Write_Register(device, MPU6050_REG_ACCEL_CONFIG, &range);
}

/**
 * This sets the divider from the gyroscope output, which is used  to set the
 * devices sample rate.
 */
HAL_StatusTypeDef MPU6050_Set_Device_Sample_Rate(MPU6050 *device,
                                                 __uint8_t rate) {
  __uint8_t reg_data;
  // First read what value the DLPF is working with to find the gyroscope output
  // rate
  MPU6050_Read_Register(device, MPU6050_REG_CONFIG, &reg_data);
  reg_data = reg_data & 0x07; // Select the important bits
  __uint8_t gyro_output_rate =
      (reg_data == 0 || reg_data == 7)
          ? 8000
          : 1000; // Declare the output rate as 8000 if DLPF_CFG is 0 or 7

  // Calculate the appropriate divider for given rate
  reg_data = (gyro_output_rate / rate) - 1;

  // Write sample rate to the device.
  return MPU6050_Write_Register(device, MPU6050_REG_SMPLRT_DIV, &reg_data);
}

/**
 * Read and report any interrupts from the device. (pg xx)
 */
HAL_StatusTypeDef MPU6050_Read_Interrupts(MPU6050 *device) {
  __uint8_t reg_data;
  MPU6050_Read_Register(device, MPU6050_REG_INT_STATUS, &reg_data);
  device->interrupts = reg_data;
  return HAL_OK;
}

/**
 * Read thermometer data. (pg 30)
 * This method will read each register individually and will place the data in
 * the devices temperature data pack after being converted.
 */
HAL_StatusTypeDef MPU6050_Read_Temperature(MPU6050 *device) {
  __uint8_t data[2];
  __int16_t temp;
  if (MPU6050_Read_Registers(device, MPU6050_REG_TEMP_OUT_H, &data[0], 2) !=
      HAL_OK) {
    return HAL_ERROR;
  }

  temp = ((__uint16_t)data[0] << 8) | data[1];

  device->temp_C = temp / 340.0 + 36.53;

  return HAL_OK;
}


/**
 * Calibrate the device by taking multiple readings and averaging them.
 * @param MPU6050* device
 * */
HAL_StatusTypeDef MPU6050_Calibrate_Device(MPU6050 *device){
	__int32_t cals[6] = {0};
	__uint8_t N = 100; // Number of calibration loops



	for (__uint8_t i = 0; i < N/10; i++){
		MPU6050_Read_All_Sensors(device);
	}

	for (__uint8_t i = 0; i < N; i++){
		MPU6050_Read_All_Sensors(device);
		for (__uint8_t j = 0; j < 6; j++){
			cals[j] += device->data[j];
		}
		HAL_Delay(10);
	}

	for (__uint8_t i = 0; i < 6; i++){
		device->calibration[i] = cals[i] / (float) N;
	}
	return HAL_OK;
}

HAL_StatusTypeDef MPU6050_Calculate_Pitch_Roll(MPU6050 *device){
	uint32_t t = HAL_GetTick();
	MPU6050_Read_All_Sensors(device);
	t = t - HAL_GetTick();
	t *= 1000;
	device->acc_pitch = 180 * atan2(device->data[0], sqrt(device->data[1]*device->data[1] + device->data[2]*device->data[2]))/PI;
	device->acc_roll = 180 * atan2(device->data[1], sqrt(device->data[0]*device->data[0] + device->data[2]*device->data[2]))/PI;

	device->gyr_pitch += device->data[1] * t;
	device->gyr_roll += device->data[0] * t;
	device->gyr_yaw += device->data[3] * t;

	device->roll = 0.96 * device->gyr_roll + 0.04 * device->acc_roll;
	device->pitch = 0.96 * device->gyr_pitch + 0.04 * device->acc_pitch;
	return HAL_OK;
}

/**
 * This method makes the device run a self test and is used to calibrate the
 * device. This method needs a more powerful Microcontroller with more memory
 * available as it needs a lot of memory to be stored.
 * */
HAL_StatusTypeDef MPU6050_Run_Self_Test(MPU6050 *device) {
  /** Gyroscope (pg 9) Accelerometer (pg 10)
   * 		1. Set the full scale range to +- 250dps and +- 8g and turn on
   * self-test modes
   * 		2. Perform the necessary calculations to obtain the Gyroscope
   * Factory Trim value (FT)
   * 		3. Obtain the Self Test Response (STR) as well as the change
   * 		4. This value must be within the specified values in the
   * datasheet: within 14% (pg12)
   * */
  __uint8_t config_data;

  MPU6050_Read_Gyroscope(device); // Read gyroscope with self test off
  MPU6050_Read_Acceleration(
      device); // Read accelerometer data with self test off

  __int16_t acc_no_st_res[3];
  __int16_t gyr_no_st_res[3];

  for (__uint8_t i; i < 3; i++) {
    acc_no_st_res[i] = device->acc[i];
    gyr_no_st_res[i] = device->rad[i];
  }

  config_data = MPU6050_GYRO_CONFIG_250DPS | MPU6050_GYRO_CONFIG_ALL_SELF_TEST;
  MPU6050_Write_Register(device, MPU6050_REG_GYRO_CONFIG, &config_data);

  config_data = MPU6050_ACCEL_CONFIG_2G | MPU6050_ACCEL_CONFIG_ALL_SELF_TEST;
  MPU6050_Write_Register(device, MPU6050_REG_ACCEL_CONFIG, &config_data);

  MPU6050_Read_Gyroscope(device); // Read gyroscope with self test on
  MPU6050_Read_Acceleration(
      device); // Read accelerometer data with self test on

  __uint8_t gyroscope_self_test_data[3];     // [XG_TEST, YG_TEST, ZG_TEST]
  __uint8_t accelerometer_self_test_data[3]; // [XA_TEST, YA_TEST, ZA_TEST]

  MPU6050_Read_Register(device, MPU6050_REG_SELF_TEST_X, &config_data);
  gyroscope_self_test_data[0] = config_data & 0x1F; // Select only bits 0-4
  accelerometer_self_test_data[0] =
      (config_data & 0xD0) >>
      3; // Shift to only have bits 5-7 in place after selecting test data

  MPU6050_Read_Register(device, MPU6050_REG_SELF_TEST_Y, &config_data);
  gyroscope_self_test_data[1] = config_data & 0x1F; // Select only bits 0-4
  accelerometer_self_test_data[1] =
      (config_data & 0xD0) >>
      3; // Shift to only have bits 5-7 in place after selecting test data

  MPU6050_Read_Register(device, MPU6050_REG_SELF_TEST_Z, &config_data);
  gyroscope_self_test_data[2] = config_data & 0x1F; // Select only bits 0-4
  accelerometer_self_test_data[2] =
      (config_data & 0xD0) >>
      3; // Shift to only have bits 5-7 in place after selecting test data

  MPU6050_Read_Register(device, MPU6050_REG_SELF_TEST_A, &config_data);
  accelerometer_self_test_data[0] +=
      ((config_data & 0x30) >>
       4); // Select bits 4-5 and add them to the test result
  accelerometer_self_test_data[1] +=
      ((config_data & 0x0C) >>
       2); // Select bits 2-3 and add them to the test result
  accelerometer_self_test_data[2] +=
      (config_data & 0x03); // Select bits 0-1 and add them to the test result

  __int16_t G[3]; // [XG, YG, ZG]
  __int16_t A[3]; // [XA, YA, ZA]

  for (__uint8_t i = 0; i < 3; i++) {
    //		G[i] = (gyroscope_self_test_data[i] == 0) ? 0 : (25 * 131 *
    // pow(1.046, gyroscope_self_test_data[i] - 1));
    G[i] = (gyroscope_self_test_data[i] == 0)
               ? 0
               : (3275 * pow(1.046, gyroscope_self_test_data[i] - 1));

    //		A[i] = (accelerometer_self_test_data[i] == 0) ? 0 : (1392.64 *
    // pow((2.7), (accelerometer_self_test_data[i] - 1)/30));
    A[i] =
        (accelerometer_self_test_data[i] == 0)
            ? 0
            : (4096 * 0.34 *
               pow((0.92 / 0.34), (accelerometer_self_test_data[i] - 1) / 30));
  }
  G[1] *= -1;
  __int8_t gyro_response[3];
  __int8_t accel_response[3];

  /* Calculating the self test response and trim  */
  for (__uint8_t i = 0; i < 3; i++) {
    gyro_response[i] =
        100 * ((device->rad[i] - gyr_no_st_res[i]) - G[i]) / G[i];
    accel_response[i] =
        100 * ((device->acc[i] - acc_no_st_res[i]) - A[i]) / A[i];
  }

  /* Turn off self test mode */
  config_data = 0x00;

  MPU6050_Write_Register(device, MPU6050_REG_GYRO_CONFIG, &config_data);
  MPU6050_Write_Register(device, MPU6050_REG_ACCEL_CONFIG, &config_data);

  for (__uint8_t i = 0; i < 3; i++) {
    if (gyro_response[i] > 14 && gyro_response[i] < -14) {
      return HAL_ERROR;
    }
    if (accel_response[i] > 14 && accel_response[i] < -14) {
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
 * @brief This method will read each register individually and will place the
 * data in the devices accelerometer data pack. Read accelerometer data. (pg 29)
 * @param MPU6050* device
 * @return OK, ERROR, BUSY
 */
HAL_StatusTypeDef MPU6050_Read_Acceleration(MPU6050 *device) {
  /* 	This method assumes that the accelerometer will spit out a signed
          integer that will be read. This is what's collected by 'val'.
  */
  __uint8_t sensor_data[6]; // All the sensor data [Ax,Ay,Az,Gx,Gy,Gz,T]

  if (MPU6050_Read_Registers(device, MPU6050_REG_ACCEL_XOUT_H, sensor_data,
                             6) != HAL_OK) {
    return HAL_ERROR;
  }

  device->acc[0] =
      (((__int16_t)sensor_data[0] << 8) | (__int16_t)sensor_data[1]) /
      device->accel_scale;
  device->acc[1] =
      (((__int16_t)sensor_data[2] << 8) | (__int16_t)sensor_data[3]) /
      device->accel_scale;
  device->acc[2] =
      (((__int16_t)sensor_data[4] << 8) | (__int16_t)sensor_data[5]) /
      device->accel_scale;

  return HAL_OK;
}

/**
 * @brief Enables the FIFO on a given MPU6050 device
 * @param MPU6050* device
 * @return OK, ERROR or BUSY
 */
HAL_StatusTypeDef MPU6050_Enable_Sensor_FIFO(MPU6050 *device) {
  __uint8_t reg_data = MPU6050_FIFO_EN_ALL_SENS;

  return MPU6050_Write_Register(device, MPU6050_REG_FIFO_EN, &reg_data);
}

/**
 * @brief Enables the a give interrupt on the MPU6050 device
 * @param MPU6050* device, __uint8_t interrupt
 * @return OK, ERROR or BUSY
 */
HAL_StatusTypeDef MPU6050_Enable_Interrupt(MPU6050 *device,
                                           __uint8_t interrupt) {
  __uint8_t reg_data = interrupt;
  if (interrupt == MPU6050_DATA_RDY) {
    device->interruptable = false;
  }
  return MPU6050_Write_Register(device, MPU6050_REG_INT_ENABLE, &reg_data);
}

/**
 * @brief Checks if data is ready from the MPU6050 device.
 * @param MPU6050* device
 * @return 1 if data is ready, otherwise return any other number
 */
bool MPU6050_Is_Data_Ready(MPU6050 *device) {
  MPU6050_Read_Interrupts(device);
  return ((device->interrupts && MPU6050_DATA_RDY) > 0);
}

/**
 * @brief Reads the data from all sensors
 * @param MPU6050* device
 * @return OK, ERROR or BUSY
 */
HAL_StatusTypeDef MPU6050_Read_All_Sensors(MPU6050 *device) {
  // Checks interrupts to see if there is any update
  while (device->interruptable && MPU6050_Is_Data_Ready(device) != 1) {
    ;
  }

  // Collect all data
  __uint8_t sensor_data[14]; // All the sensor data [Ax,Ay,Az,Gx,Gy,Gz,T]

  if (MPU6050_Read_Registers(device, MPU6050_REG_ACCEL_XOUT_H, sensor_data,
                             14) != HAL_OK) {
    return HAL_ERROR;
  }

  device->acc[0] =
      (((__int16_t)sensor_data[0] << 8) | (__int16_t)sensor_data[1]) /
      device->accel_scale;
  device->acc[1] =
      (((__int16_t)sensor_data[2] << 8) | (__int16_t)sensor_data[3]) /
      device->accel_scale;
  device->acc[2] =
      (((__int16_t)sensor_data[4] << 8) | (__int16_t)sensor_data[5]) /
      device->accel_scale;

  device->rad[0] =
      (((__int16_t)sensor_data[8] << 8) | (__int16_t)sensor_data[9]) /
      device->gyro_scale;
  device->rad[1] =
      (((__int16_t)sensor_data[10] << 8) | (__int16_t)sensor_data[11]) /
      device->gyro_scale;
  device->rad[2] =
      (((__int16_t)sensor_data[12] << 8) | (__int16_t)sensor_data[13]) /
      device->gyro_scale;

  __int16_t temp = ((__int16_t)sensor_data[6] << 8) | (__int16_t)sensor_data[7];

  device->temp_C = temp / 340.0 + 36.53;

  for (__uint8_t i = 0; i < 7; i++){
	  device->prev_data[i] = device->data[i];
  }

  device->data[0] = device->acc[0];
  device->data[1] = device->acc[1];
  device->data[2] = device->acc[2];
  device->data[3] = device->rad[0];
  device->data[4] = device->rad[1];
  device->data[5] = device->rad[2];
  device->data[6] = device->temp_C;


  for (__uint8_t i = 0; i < 7; i++){
	  device->data[i] = device->data[i] - device->calibration[i];
  }
//  MPU6050_Calculate_Pitch_Roll(device);

  return HAL_OK;
}

/**
 * Read gyroscope data. (pg 31)
 * This method will read each register individually and will place the data in
 * the devices gyroscope data pack after being converted.
 */
HAL_StatusTypeDef MPU6050_Read_Gyroscope(MPU6050 *device) {
  __uint8_t sensor_data[6]; // All the sensor data [Ax,Ay,Az,Gx,Gy,Gz,T]

  // Burst reads the 6 bytes of data
  if (MPU6050_Read_Registers(device, MPU6050_REG_GYRO_XOUT_H, sensor_data, 6) !=
      HAL_OK) {
    return HAL_ERROR;
  }

  device->rad[0] =
      (((__int16_t)sensor_data[0] << 8) | (__int16_t)sensor_data[1]) /
      device->gyro_scale;
  device->rad[1] =
      (((__int16_t)sensor_data[2] << 8) | (__int16_t)sensor_data[3]) /
      device->gyro_scale;
  device->rad[2] =
      (((__int16_t)sensor_data[4] << 8) | (__int16_t)sensor_data[5]) /
      device->gyro_scale;

  return HAL_OK;
}

/**
 * @brief Reads a byte from the device's given register
 * @param MPU6050* device
 * @param __uint8_t bandwidth as defined by MPU6050_CONFIG_DLPF_*
 * @return OK, ERROR, BUSY
 */
HAL_StatusTypeDef MPU6050_Set_Filter_Bandwidth(MPU6050 *device,
                                               __uint8_t bandwidth) {
  __uint8_t data;

  MPU6050_Read_Register(device, MPU6050_REG_CONFIG, &data);

  data = (data & 0xF8) | bandwidth; // Clear DLPF bits and assign new value
  return MPU6050_Write_Register(device, MPU6050_REG_CONFIG, &data);
}

/**
 * @brief Reads a byte from the device's given register
 * @param MPU6050* device
 * @param __uint8_t register
 * @param __uint8_t* pointer to data
 * @return OK, ERROR or BUSY
 */
HAL_StatusTypeDef MPU6050_Read_Register(MPU6050 *device, __uint8_t reg,
                                        __uint8_t *data) {
  return HAL_I2C_Mem_Read(device->i2cHandle, MPU6050_I2C_ADDR, reg,
                          I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

/**
 * @brief Reads a given number of bytes from a device via I2C. Works if device a
 * device will auto increment its counter
 * @param MPU6050* device
 * @param __uint8_t register
 * @param __uint8_t* pointer to data
 * @param __uint8_t number of bytes to read
 * @return OK, ERROR or BUSY
 */
HAL_StatusTypeDef MPU6050_Read_Registers(MPU6050 *device, __uint8_t reg,
                                         __uint8_t *data, __uint8_t length) {
  return HAL_I2C_Mem_Read(device->i2cHandle, MPU6050_I2C_ADDR, reg,
                          I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

/**
 * @brief Writes data to a device via I2C
 * @param MPU6050* device
 * @param __uint8_t register
 * @param __uint8_t* pointer to data
 * @return OK, ERROR or BUSY
 */
HAL_StatusTypeDef MPU6050_Write_Register(MPU6050 *device, __uint8_t reg,
                                         __uint8_t *data) {
  // return HAL_I2C_Mem_Write(device->i2cHandle, MPU6050_I2C_ADDR, reg,
  //                          I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
  return i2c_smbus_write_byte_data(int device->i2cHandle, reg, data);
}
