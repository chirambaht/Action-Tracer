#include <cstdint>
#include "action_definitions.pb.h"

#define MAX_ACT_DEVICES				   13
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

#define ACT_DATA_TEMPERATURE  ( 1 << 0 )
#define ACT_DATA_QUATERNION	  ( 1 << 1 )
#define ACT_DATA_ACCELERATION ( 1 << 2 )
#define ACT_DATA_GYROSCOPE	  ( 1 << 3 )
#define ACT_DATA_EULER		  ( 1 << 4 )
#define ACT_DATA_AIRCRAFT	  ( 1 << 5 )
#define ACT_DATA_GRAVITY	  ( 1 << 6 )

#define ACT_0  0
#define ACT_1  ( 1 << 0 )
#define ACT_2  ( 1 << 1 )
#define ACT_3  ( 1 << 2 )
#define ACT_4  ( 1 << 3 )
#define ACT_5  ( 1 << 4 )
#define ACT_6  ( 1 << 5 )
#define ACT_7  ( 1 << 6 )
#define ACT_8  ( 1 << 7 )
#define ACT_9  ( 1 << 8 )
#define ACT_10 ( 1 << 9 )
#define ACT_11 ( 1 << 10 )
#define ACT_12 ( 1 << 11 )
#define ACT_13 ( 1 << 12 )

#define ACT_BODY_WAIST		   0
#define ACT_BODY_RIGHT_BICEP   ( 1 << 0 )
#define ACT_BODY_RIGHT_FOREARM ( 1 << 1 )
#define ACT_BODY_RIGHT_HAND	   ( 1 << 2 )
#define ACT_BODY_LEFT_BICEP	   ( 1 << 3 )
#define ACT_BODY_LEFT_FOREARM  ( 1 << 4 )
#define ACT_BODY_LEFT_HAND	   ( 1 << 5 )
#define ACT_BODY_CHEST		   ( 1 << 6 )
#define ACT_BODY_HEAD		   ( 1 << 7 )
#define ACT_BODY_RIGHT_THIGH   ( 1 << 8 )
#define ACT_BODY_RIGHT_KNEE	   ( 1 << 9 )
#define ACT_BODY_RIGHT_FOOT	   ( 1 << 10 )
#define ACT_BODY_LEFT_THIGH	   ( 1 << 11 )
#define ACT_BODY_LEFT_KNEE	   ( 1 << 12 )
#define ACT_BODY_LEFT_FOOT	   ( 1 << 13 )
#define ACT_BODY_RIGHT_HIP	   ( 1 << 14 )
#define ACT_BODY_LEFT_HIP	   ( 1 << 15 )

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
#define MAX_CLIENTS	 10

namespace ActionTracer {
	typedef struct ActionDataPackage {
		// [ 24-bit identifier | 8-bit contents ]
		__uint32_t device_identifier_contents = 0;

		float data[DATA_ELEMENTS];
	} ActionDataPackage;

} // namespace ActionTracer
