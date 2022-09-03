from re import I


MAX_ACT_DEVICES				   = 17

ACT_I2C_DATA_WIRING_PI_PIN	=8
ACT_I2C_CLOCK_WIRING_PI_PIN =9

ACT_DEVICE_0_WIRING_PI_PIN	=7
ACT_DEVICE_1_WIRING_PI_PIN	=0
ACT_DEVICE_2_WIRING_PI_PIN	=2
ACT_DEVICE_3_WIRING_PI_PIN	=3
ACT_DEVICE_4_WIRING_PI_PIN	=12
ACT_DEVICE_5_WIRING_PI_PIN	=13
ACT_DEVICE_6_WIRING_PI_PIN	=14
ACT_DEVICE_7_WIRING_PI_PIN	=30
ACT_DEVICE_8_WIRING_PI_PIN	=21
ACT_DEVICE_9_WIRING_PI_PIN	=22
ACT_DEVICE_10_WIRING_PI_PIN =23
ACT_DEVICE_11_WIRING_PI_PIN =24
ACT_DEVICE_12_WIRING_PI_PIN =25

ACT_DEVICE_0_INT_WIRING_PI_PIN= 27

ACT_DATA_TEMPERATURE  =( 1 << 0 )
ACT_DATA_QUATERNION	  =( 1 << 1 )
ACT_DATA_ACCELERATION =( 1 << 2 )
ACT_DATA_GYROSCOPE	  =( 1 << 3 )
ACT_DATA_EULER		  =( 1 << 4 )
ACT_DATA_AIRCRAFT	  =( 1 << 5 )
ACT_DATA_GRAVITY	  =( 1 << 6 )

ACT_0  =0
ACT_1  =( 1 << 0 )
ACT_2  =( 1 << 1 )
ACT_3  =( 1 << 2 )
ACT_4  =( 1 << 3 )
ACT_5  =( 1 << 4 )
ACT_6  =( 1 << 5 )
ACT_7  =( 1 << 6 )
ACT_8  =( 1 << 7 )
ACT_9  =( 1 << 8 )
ACT_10 =( 1 << 9 )
ACT_11 =( 1 << 10 )
ACT_12 =( 1 << 11 )
ACT_13 =( 1 << 12 )

ACT_BODY_WAIST		   =0
ACT_BODY_RIGHT_BICEP   =( 1 << 0 )
ACT_BODY_RIGHT_FOREARM =( 1 << 1 )
ACT_BODY_RIGHT_HAND	   =( 1 << 2 )
ACT_BODY_LEFT_BICEP	   =( 1 << 3 )
ACT_BODY_LEFT_FOREARM  =( 1 << 4 )
ACT_BODY_LEFT_HAND	   =( 1 << 5 )
ACT_BODY_CHEST		   =( 1 << 6 )
ACT_BODY_HEAD		   =( 1 << 7 )
ACT_BODY_RIGHT_THIGH   =( 1 << 8 )
ACT_BODY_RIGHT_KNEE	   =( 1 << 9 )
ACT_BODY_RIGHT_FOOT	   =( 1 << 10 )
ACT_BODY_LEFT_THIGH	   =( 1 << 11 )
ACT_BODY_LEFT_KNEE	   =( 1 << 12 )
ACT_BODY_LEFT_FOOT	   =( 1 << 13 )
ACT_BODY_RIGHT_HIP	   =( 1 << 14 )
ACT_BODY_LEFT_HIP	   =( 1 << 15 )

DEVICES_IN_USE	   = 3				   # 3 IMUs
DATA_ELEMENTS	   = DEVICES_IN_USE * 20 # 20 data elements per IMU
PACKAGE_LENGTH	   = DATA_ELEMENTS + 3
PACKAGE_DATA_START = 3 # 3 is the number of elements in the header

DEFAULT_PORT = 9022

def partFromIdentifier( identifier ):
    if identifier == ACT_BODY_WAIST:
        return "WAIST"
    elif identifier == ACT_BODY_RIGHT_BICEP:
        return "RIGHT_BICEP"
    elif identifier == ACT_BODY_RIGHT_FOREARM:
        return "RIGHT_FOREARM"
    elif identifier == ACT_BODY_RIGHT_HAND:
        return "RIGHT_HAND"
    elif identifier == ACT_BODY_LEFT_BICEP:
        return "LEFT_BICEP"
    elif identifier == ACT_BODY_LEFT_FOREARM:
        return "LEFT_FOREARM"
    elif identifier == ACT_BODY_LEFT_HAND:
        return "LEFT_HAND"
    elif identifier == ACT_BODY_CHEST:
        return "CHEST"
    elif identifier == ACT_BODY_HEAD:
        return "HEAD"
    elif identifier == ACT_BODY_RIGHT_THIGH:
        return "RIGHT_THIGH"
    elif identifier == ACT_BODY_RIGHT_KNEE:
        return "RIGHT_KNEE"
    elif identifier == ACT_BODY_RIGHT_FOOT:
        return "RIGHT_FOOT"
    elif identifier == ACT_BODY_LEFT_THIGH:
        return "LEFT_THIGH"
    elif identifier == ACT_BODY_LEFT_KNEE:
        return "LEFT_KNEE"
    elif identifier == ACT_BODY_LEFT_FOOT:
        return "LEFT_FOOT"
    elif identifier == ACT_BODY_RIGHT_HIP:
        return "RIGHT_HIP"
    elif identifier == ACT_BODY_LEFT_HIP:
        return "LEFT_HIP"
    else:
        return ""

