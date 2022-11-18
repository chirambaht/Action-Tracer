#ifndef ACT_DEFINITIONS_H
#define ACT_DEFINITIONS_H

#include "action_definitions.pb.h"

#include <cstdint>

#define MAX_ACT_DEVICES				   17
#define SINGLE_ACT_DEVICE_PACKAGE_SIZE ( MAXIMUM_COLLECTED_DATA_POINTS * sizeof( float ) )
#define DATA_PACKAGE_SIZE			   ( MAX_ACT_DEVICES * SINGLE_ACT_DEVICE_PACKAGE_SIZE )

#define ACT_I2C_DATA_WIRING_PI_PIN	8
#define ACT_I2C_CLOCK_WIRING_PI_PIN 9

#define ACT_DEVICE_0_WIRING_PI_PIN	7
#define ACT_DEVICE_1_WIRING_PI_PIN	0
#define ACT_DEVICE_2_WIRING_PI_PIN	2
#define ACT_DEVICE_3_WIRING_PI_PIN	3
#define ACT_DEVICE_4_WIRING_PI_PIN	12
#define ACT_DEVICE_5_WIRING_PI_PIN	13
#define ACT_DEVICE_6_WIRING_PI_PIN	14
#define ACT_DEVICE_7_WIRING_PI_PIN	30
#define ACT_DEVICE_8_WIRING_PI_PIN	21
#define ACT_DEVICE_9_WIRING_PI_PIN	22
#define ACT_DEVICE_10_WIRING_PI_PIN 23
#define ACT_DEVICE_11_WIRING_PI_PIN 24
#define ACT_DEVICE_12_WIRING_PI_PIN 25

#define ACT_DEVICE_0_INT_WIRING_PI_PIN 27

#define ACT_DATA_TEMPERATURE  ( 1u << 0 )
#define ACT_DATA_QUATERNION	  ( 1u << 1 )
#define ACT_DATA_ACCELERATION ( 1u << 2 )
#define ACT_DATA_GYROSCOPE	  ( 1u << 3 )
#define ACT_DATA_EULER		  ( 1u << 4 )
#define ACT_DATA_AIRCRAFT	  ( 1u << 5 )
#define ACT_DATA_GRAVITY	  ( 1u << 6 )

#define ACT_0  0
#define ACT_1  ( 1u << 0 )
#define ACT_2  ( 1u << 1 )
#define ACT_3  ( 1u << 2 )
#define ACT_4  ( 1u << 3 )
#define ACT_5  ( 1u << 4 )
#define ACT_6  ( 1u << 5 )
#define ACT_7  ( 1u << 6 )
#define ACT_8  ( 1u << 7 )
#define ACT_9  ( 1u << 8 )
#define ACT_10 ( 1u << 9 )
#define ACT_11 ( 1u << 10 )
#define ACT_12 ( 1u << 11 )
#define ACT_13 ( 1u << 12 )

#define ACT_BODY_WAIST		   0
#define ACT_BODY_RIGHT_BICEP   ( 1u << 0 )
#define ACT_BODY_RIGHT_FOREARM ( 1u << 1 )
#define ACT_BODY_RIGHT_HAND	   ( 1u << 2 )
#define ACT_BODY_LEFT_BICEP	   ( 1u << 3 )
#define ACT_BODY_LEFT_FOREARM  ( 1u << 4 )
#define ACT_BODY_LEFT_HAND	   ( 1u << 5 )
#define ACT_BODY_CHEST		   ( 1u << 6 )
#define ACT_BODY_HEAD		   ( 1u << 7 )
#define ACT_BODY_RIGHT_THIGH   ( 1u << 8 )
#define ACT_BODY_RIGHT_KNEE	   ( 1u << 9 )
#define ACT_BODY_RIGHT_FOOT	   ( 1u << 10 )
#define ACT_BODY_LEFT_THIGH	   ( 1u << 11 )
#define ACT_BODY_LEFT_KNEE	   ( 1u << 12 )
#define ACT_BODY_LEFT_FOOT	   ( 1u << 13 )
#define ACT_BODY_RIGHT_HIP	   ( 1u << 14 )
#define ACT_BODY_LEFT_HIP	   ( 1u << 15 )

#ifndef DEVICES_IN_USE
	#define DEVICES_IN_USE	   3				   // 3 IMUs
	#define DATA_ELEMENTS	   DEVICES_IN_USE * 20 // 20 data elements per IMU
	#define PACKAGE_LENGTH	   DATA_ELEMENTS + 3
	#define PACKAGE_DATA_START 3 // 3 is the number of elements in the header

/*
	|  HEADER  |           DATA           |
	|  0 -> 2  |  3 -> DATA_ELEMENTS - 1  |

	HEADER 0: Time
	HEADER 1: Count
	HEADER 2: Devices Connected
*/
#endif

#define DEFAULT_PORT 9022
#define MAX_CLIENTS	 1

namespace ActionTracer {
	typedef struct ActionDataPackage {
		// [ 32-bit identifier ]
		__uint32_t device_identifier_contents = 0;

		float data[11]; // Accel (3), Gyro (3), Quaternion(4), Temp (1)
	} ActionDataPackage;

} // namespace ActionTracer
#endif