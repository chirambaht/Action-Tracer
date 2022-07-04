#include "tracer_point.h"

#ifdef DEBUG
	#define debugPrint( ... )	printf( __VA_ARGS__ )
	#define debugPrintln( ... ) printf( __VA_ARGS__ )
#else
	#define debugPrint( ... )
	#define debugPrintln( ... )
#endif

#ifdef ON_PI
	#include <wiringPi.h>
#endif

// Select the data you want out of the senser here. Only select one.
#define GET_DATA_QUATERNION
// #define GET_DATA_EULER
// #define GET_DATA_ALL
// #define GET_DATA_GYROSCOPE
// #define GET_DATA_ACCELEROMETER
// #define GET_DATA_YAWPITCHROLL

using namespace ActionTracer;
/**
 * @brief Construct a new defult Action Tracer::Trace Point::Trace Point object
 *
 */
ActionTracer::TracePoint::TracePoint() {}

/**
 * @brief Construct a new Action Tracer:: TracePoint::Trace Point object.
 *
 * @param identifier Identifier given to the device
 * @param wiring_Pi_pin_number Slave select pin on Raspberry pi according to WiringPi.
 * @param interrupt_pin WiringPi interrupt pin
 */
ActionTracer::TracePoint::TracePoint( uint8_t identifier, uint8_t wiring_Pi_pin_number, uint8_t interrupt_pin ) {
	debugPrintln( "Constructing the device as is needed. Name = %s\n", name.c_str() );

	_identifier = identifier;

	_pin_number = wiring_Pi_pin_number;

// Set pin information
#ifdef ON_PI
	pinMode( _pin_number, OUTPUT );
#endif
	_device_interrupt_flag = false;

	debugPrint( "Initializing %s...\n", _device_name.c_str() );

	this->_select_me();
	_device = new MPU6050( MPU6050_ADDRESS_AD0_HIGH );

	_device->initialize();

	// TODO: At this stage an interrupt pin is initialised

	debugPrint( _device->testConnection() ? "%s connection successful\n" : "%s connection failed\n", _device_name.c_str() );

	// DMP Initialization

	debugPrint( "Initalising DMP\n" );
	_device_status = _device->dmpInitialize();

	_set_device_offsets();

	if( _device_status == 0 ) {
		debugPrint( "Enabling DMP..." );
		_device->setDMPEnabled( true );

		// TODO: Attach interrupt here

		_device_interrupt_status = _device->getIntStatus();

		_dmp_ready = true;

		_packet_size = _device->dmpGetFIFOPacketSize();

		debugPrint( "Enabled!\n" );
	} else {
		debugPrint( "Can't initialise DMP\n" );
		_dmp_ready = false;
	}

	this->_deselect_me();

	debugPrint( "Init variable dump\n" );

	debugPrint( "\n\tDevice Name:\t\t%s\n\tPin number:\t\t%d\n\tDMP Status:\t\t%d\n\tFIFO Packet Size:\t%d\n", _device_name.c_str(), _pin_number, _dmp_ready, _packet_size );
}

/**
 * @brief Construct a new Action Tracer:: TracePoint::Trace Point object.
 *
 * @param name Name given to the device
 * @param wiring_Pi_pin_number Slave select pin on Raspberry pi according to WiringPi.
 */
ActionTracer::TracePoint::TracePoint( uint8_t identifier, uint8_t wiring_Pi_pin_number ) {
	debugPrintln( "Constructing the device as is needed. Name = %s\n", name.c_str() );

	_identifier = identifier;

	_pin_number = wiring_Pi_pin_number;

// Set pin information
#ifdef ON_PI
	pinMode( _pin_number, OUTPUT );
#endif

	_device_interrupt_flag = false;

	debugPrint( "Initializing %s...\n", _device_name.c_str() );

	this->_select_me();

	_device = new MPU6050( MPU6050_ADDRESS_AD0_HIGH );

	_device->initialize();

	// TODO: At this stage an interrupt pin is initialised

	debugPrint( _device->testConnection() ? "%s connection successful\n" : "%s connection failed\n", _device_name.c_str() );

	// DMP Initialization

	debugPrint( "Initalising DMP\n" );
	_device_status = _device->dmpInitialize();

	_set_device_offsets();

	if( _device_status == 0 ) {
		debugPrint( "Enabling DMP..." );
		_device->setDMPEnabled( true );

		// TODO: Attach interrupt here

		_dmp_ready = true;

		_packet_size = _device->dmpGetFIFOPacketSize();

		debugPrint( "Enabled!\n" );
	} else {
		debugPrint( "Can't initialise DMP\n" );
		_dmp_ready = false;
	}

	this->_deselect_me();

	debugPrint( "Init variable dump\n" );

	debugPrint( "\tDevice Name:\t\t%s\n\tPin number:\t\t%d\n\tDMP Status:\t\t%d\n\tFIFO Packet Size:\t%d\n\n", _device_name.c_str(), _pin_number, _dmp_ready, _packet_size );
}

/**
 * @brief Selects a given MPU6050 node. Must be deselected to avoid issues.
 */
void ActionTracer::TracePoint::_select_me() {
#ifdef ON_PI
	digitalWrite( _pin_number, HIGH );
#endif
}

/**
 * @brief Deselects a given MPU6050 node. Public function to avoid issues.
 */
void ActionTracer::TracePoint::turn_off() {
	_deselect_me();
}

/**
 * @brief Deselects a given MPU6050 node.
 */
void ActionTracer::TracePoint::_deselect_me() {
#ifdef ON_PI
	digitalWrite( _pin_number, LOW );
#endif
}

/** @brief Calls on the selected sensor to identify itself by returning its name. It will physically indicate this by blinking an onboard LED.
 * @return Device name/id
 */
uint8_t ActionTracer::TracePoint::identify() {
// Blink device led
#ifdef ON_PI
	for( size_t i = 0; i < 5; i++ ) {
		_select_me();
		delay( 250 );
		_deselect_me();
		delay( 250 );
	}
#endif

	return _identifier;
}

/**
 * @brief Prints the last data packet received if debug mode is on.
 * @return Nothing
 *
 */
void ActionTracer::TracePoint::print_last_data_packet() {
#ifdef GET_DATA_QUATERNION
	debugPrint( "Output data type: Quaternion\nLast packet was: %5f, %5f, %5f, %5f\n", _quaternion_float_packet[0], _quaternion_float_packet[1], _quaternion_float_packet[2], _quaternion_float_packet[3] );
#endif

#ifdef GET_DATA_EULER
	debugPrint( "Output data type: Euler\nLast packet was: %5f, %5f, %5f\n", _euler_packet[0], _euler_packet[1], _euler_packet[2] );
#endif

#ifdef GET_DATA_ACCELEROMETER
	debugPrint( "Output data type: Accelerometer\nLast packet was: %5f, %5f, %5f\n", _acceleration_float_packet[0], _acceleration_float_packet[1], _acceleration_float_packet[2] );
#endif

#ifdef GET_DATA_GYROSCOPE
	debugPrint( "Output data type: Gyroscope\nLast packet was: %5f, %5f, %5f\n", _gyroscope_float_packet[0], _gyroscope_float_packet[1], _gyroscope_float_packet[2] );
#endif

#ifdef GET_DATA_YAWPITCHROLL
	debugPrint( "Output data type: Yaw, Pitch and Roll\nLast packet was: %5f, %5f, %5f\n", _yaw_pitch_roll_packet[0], _yaw_pitch_roll_packet[1], _yaw_pitch_roll_packet[2] );
#endif
}

/**
 * @brief Obtain the data from the sensor. Collects the FIFO packet and extracts the needed data.
 *
 */
void ActionTracer::TracePoint::get_data() {
	if( !_dmp_ready ) {
		debugPrint( "%s: DMP not initialised\n", _device_name.c_str() );
		this->_deselect_me();
		return;
	}

	this->_select_me();

	_device_interrupt_status = _device->getIntStatus();

	// does the FIFO have data in it?
	if( ( _device_interrupt_status & 0x02 ) < 1 ) {
		debugPrintln( "%s: Interrupt not raised\n", _device_name.c_str() );
		this->_deselect_me();
		return;
	}

	_fifo_count = _device->getFIFOCount();

	if( _fifo_count < _packet_size ) {
		debugPrintln( "%s: MPU interrupt not ready or not enough elements in FIFO\n", _device_name.c_str() );
		this->_deselect_me();
		return;
	}

	if( _fifo_count == 1024 ) {
		// reset so we can continue cleanly
		_device->resetFIFO();

		debugPrint( "%s: FIFO overflow!\n", _device_name.c_str() );
		this->_deselect_me();
		return;
	}

	_device->getFIFOBytes( _fifo_buffer, _packet_size );

	/* ================================================================================================ *
	 | Default MotionApps v2.0 42-byte FIFO packet structure:                                           |
	 |                                                                                                  |
	 | [QUAT W][      ][QUAT X][      ][QUAT Y][      ][QUAT Z][      ][GYRO X][      ][GYRO Y][      ] |
	 |   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  |
	 |                                                                                                  |
	 | [GYRO Z][      ][ACC X ][      ][ACC Y ][      ][ACC Z ][      ][      ]                         |
	 |  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41                          |
	 * ================================================================================================ */

#ifdef GET_DATA_TEAPOT
	_teapot_raw_packet[0] = ( ( _fifo_buffer[0] << 8 ) | _fifo_buffer[1] );	  // Quat W
	_teapot_raw_packet[1] = ( ( _fifo_buffer[4] << 8 ) | _fifo_buffer[5] );	  // Quat X
	_teapot_raw_packet[2] = ( ( _fifo_buffer[8] << 8 ) | _fifo_buffer[9] );	  // Quat Y
	_teapot_raw_packet[3] = ( ( _fifo_buffer[12] << 8 ) | _fifo_buffer[13] ); // Quat Z
	_teapot_raw_packet[4] = ( _fifo_buffer[16] << 8 ) | _fifo_buffer[17];	  // GYR X
	_teapot_raw_packet[5] = ( _fifo_buffer[20] << 8 ) | _fifo_buffer[21];	  // GYR Y
	_teapot_raw_packet[6] = ( _fifo_buffer[24] << 8 ) | _fifo_buffer[25];	  // GYR Z
	_teapot_raw_packet[7] = ( _fifo_buffer[28] << 8 ) | _fifo_buffer[29];	  // Acc X
	_teapot_raw_packet[8] = ( _fifo_buffer[32] << 8 ) | _fifo_buffer[33];	  // Acc Y
	_teapot_raw_packet[9] = ( _fifo_buffer[36] << 8 ) | _fifo_buffer[37];	  // Acc Z
#endif

#ifdef GET_DATA_QUATERNION
	_device->dmpGetQuaternion( &_quaternion_packet, _fifo_buffer );
	_quaternion_float_packet[0] = _quaternion_packet.w;
	_quaternion_float_packet[1] = _quaternion_packet.x;
	_quaternion_float_packet[2] = _quaternion_packet.y;
	_quaternion_float_packet[3] = _quaternion_packet.z;
#endif

#ifdef GET_DATA_EULER
	_device->dmpGetQuaternion( &_quaternion_packet, _fifo_buffer );
	_device->dmpGetEuler( &_euler_packet[0], &_quaternion_packet );
#endif

#ifdef GET_DATA_ACCELEROMETER
	_device->dmpGetAccel( &_acceleration_packet, _fifo_buffer );

	_acceleration_float_packet[0] = _acceleration_packet.x;
	_acceleration_float_packet[1] = _acceleration_packet.y;
	_acceleration_float_packet[2] = _acceleration_packet.z;
#endif

#ifdef GET_DATA_GYROSCOPE
	_device->dmpGetGyro( &_gyroscope_packet, _fifo_buffer );

	_gyroscope_float_packet[0] = _gyroscope_packet.x;
	_gyroscope_float_packet[1] = _gyroscope_packet.y;
	_gyroscope_float_packet[2] = _gyroscope_packet.z;
#endif

#ifdef GET_DATA_YAWPITCHROLL
	_device->dmpGetQuaternion( &_quaternion_packet, _fifo_buffer );
	_device->dmpGetGravity( &_gravity_packet, &_quaternion_packet );
	_device->dmpGetYawPitchRoll( &_yaw_pitch_roll_packet[0], &_quaternion_packet, &_gravity_packet );
#endif
	this->_deselect_me();
}

/**
 * @brief Obtain the data from the sensor. This will return a float array based on the requested data defined in the file.
 * @param read_first Will collect data first if set to 1 or true. After this, it will return the data
 * @return Pointer to a float array with the data packet
 */
float *ActionTracer::TracePoint::read_data( int read_first = 0 ) {
	if( read_first ) {
		this->get_data();
	}

#ifdef GET_DATA_TEAPOT
	return _teapot_raw_packet;
#endif
#ifdef GET_DATA_QUATERNION
	return _quaternion_float_packet;
#endif
#ifdef GET_DATA_EULER
	return _euler_packet;
#endif
#ifdef GET_DATA_ACCELEROMETER
	return _acceleration_float_packet;
#endif
#ifdef GET_DATA_GYROSCOPE
	return _gyroscope_float_packet;
#endif
#ifdef GET_DATA_YAWPITCHROLL
	return _yaw_pitch_roll_packet;
#endif
}

/**
 * @brief Sets device offsets based on a pointers.csv file. This file should contain the callibration details of each device. Such a csv can be obtained by running the IMU_Zero program in the Offsts directory
 * @return Nothing
 */
void ActionTracer::TracePoint::_set_device_offsets() {
	// Get new offset code
	_set_default_device_offsets();
}

/**
 * @brief Sets default values for the offsets on a sensor.
 * @return Nothing
 */
void ActionTracer::TracePoint::_set_default_device_offsets() {
	_device->setXAccelOffset( 43 );
	_device->setYAccelOffset( 25 );
	_device->setZAccelOffset( 73 );
	_device->setXGyroOffset( -17 );
	_device->setYGyroOffset( 1477 );
	_device->setZGyroOffset( 4971 );
}
