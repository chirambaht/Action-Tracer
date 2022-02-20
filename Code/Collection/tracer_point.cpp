#include "tracer_point.h"

#include <fstream>
#include <sstream>

#define debugPrint( ... )	printf( __VA_ARGS__ )
#define debugPrintln( ... ) printf( __VA_ARGS__ )

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
 * @param name Name given to the device
 * @param wiring_Pi_pin_number Slave select pin on Raspberry pi according to WiringPi.
 * @param interrupt_pin WiringPi interrupt pin
 */
ActionTracer::TracePoint::TracePoint( std::string name, int wiring_Pi_pin_number, int interrupt_pin ) {
	if( _debug )
		debugPrintln( "Constructing the device as is needed. Name = %s\n", name.c_str() );

	_device_name = name;

	_pin_number = wiring_Pi_pin_number;

// Set pin information
#ifdef ON_PI
	pinMode( _pin_number, OUTPUT );
#endif
	_device_interrupt_flag = false;

	if( _debug )
		debugPrint( "Initializing %s...\n", _device_name.c_str() );

	this->_select_me();
	_device = new MPU6050( MPU6050_ADDRESS_AD0_LOW );

	_device->initialize();

	// TODO: At this stage an interrupt pin is initialised

	if( _debug )
		debugPrint( _device->testConnection() ? "%s connection successful\n" : "%s connection failed\n", _device_name.c_str() );

	// DMP Initialization
	if( _debug )

		debugPrint( "Initalising DMP\n" );
	_device_status = _device->dmpInitialize();

	_set_device_offsets();

	if( _device_status == 0 ) {
		if( _debug )

			debugPrint( "Enabling DMP..." );
		_device->setDMPEnabled( true );

		// TODO: Attach interrupt here

		_device_interrupt_status = _device->getIntStatus();

		_dmp_ready = true;

		_packet_size = _device->dmpGetFIFOPacketSize();

		if( _debug )
			debugPrint( "Enabled!\n" );
	} else {
		if( _debug )
			debugPrint( "Can't initialise DMP\n" );
		_dmp_ready = false;
	}

	this->_deselect_me();

	if( _debug )

		debugPrint( "Init variable dump\n" );
	if( _debug )

		debugPrint( "\n\tDevice Name:\t\t%s\n\tPin number:\t\t%d\n\tDMP Status:\t\t%d\n\tFIFO Packet Size:\t%d\n", _device_name.c_str(), _pin_number, _dmp_ready, _packet_size );
}
/**
 * @brief Construct a new Action Tracer:: TracePoint::Trace Point object.
 *
 * @param name Name given to the device
 * @param wiring_Pi_pin_number Slave select pin on Raspberry pi according to WiringPi.
 */
ActionTracer::TracePoint::TracePoint( std::string name, int wiring_Pi_pin_number ) {
	if( _debug )
		debugPrintln( "Constructing the device as is needed. Name = %s\n", name.c_str() );

	_device_name = name;

	_pin_number = wiring_Pi_pin_number;

// Set pin information
#ifdef ON_PI
	pinMode( _pin_number, OUTPUT );
#endif
	_device_interrupt_flag = false;

	if( _debug )
		debugPrint( "Initializing %s...\n", _device_name.c_str() );

	this->_select_me();
	_device = new MPU6050( MPU6050_ADDRESS_AD0_LOW );

	_device->initialize();

	// TODO: At this stage an interrupt pin is initialised

	if( _debug )
		debugPrint( _device->testConnection() ? "%s connection successful\n" : "%s connection failed\n", _device_name.c_str() );

	// DMP Initialization
	if( _debug )

		debugPrint( "Initalising DMP\n" );
	_device_status = _device->dmpInitialize();

	_set_device_offsets();

	if( _device_status == 0 ) {
		if( _debug )

			debugPrint( "Enabling DMP..." );
		_device->setDMPEnabled( true );

		// TODO: Attach interrupt here

		_dmp_ready = true;

		_packet_size = _device->dmpGetFIFOPacketSize();

		if( _debug )
			debugPrint( "Enabled!\n" );
	} else {
		if( _debug )
			debugPrint( "Can't initialise DMP\n" );
		_dmp_ready = false;
	}

	this->_deselect_me();

	if( _debug )

		debugPrint( "Init variable dump\n" );
	if( _debug )

		debugPrint( "\n\tDevice Name:\t\t%s\n\tPin number:\t\t%d\n\tDMP Status:\t\t%d\n\tFIFO Packet Size:\t%d\n", _device_name.c_str(), _pin_number, _dmp_ready, _packet_size );
}

/**
 * @brief Selects a given MPU6050 node. Must be deselected to avoid issues.
 */
void ActionTracer::TracePoint::_select_me() {
#ifdef ON_PI
	digitalWrite( _pin_number, LOW );
#endif
}

/**
 * @brief Deselects a given MPU6050 node.
 */
void ActionTracer::TracePoint::_deselect_me() {
#ifdef ON_PI
	digitalWrite( _pin_number, HIGH );
#endif
}

/** @brief Calls on the selected sensor to identify itself by returning its name. It will physically indicate this by blinking an onboard LED.
 * @return Device name/id
 */
std::string ActionTracer::TracePoint::identify() {
// Blink device led
#ifdef ON_PI
	for( size_t i = 0; i < 5; i++ ) {
		_select_me();
		delay( 250 );
		_deselect_me();
		delay( 250 );
	}
#endif

	return _device_name;
}

/**
 * @brief Prints the last data packet received if debug mode is on.
 * @return Nothing
 *
 */
void ActionTracer::TracePoint::print_last_data_packet() {
#ifdef GET_DATA_QUATERNION
	if( _debug )
		debugPrint( "Output data type: Quaternion\nLast packet was: %5f, %5f, %5f, %5f\n", _quaternion_float_packet[0], _quaternion_float_packet[1], _quaternion_float_packet[2], _quaternion_float_packet[3] );
#endif

#ifdef GET_DATA_EULER
	if( _debug )

		debugPrint( "Output data type: Euler\nLast packet was: %5f, %5f, %5f\n", _euler_packet[0], _euler_packet[1], _euler_packet[2] );
#endif

#ifdef GET_DATA_ACCELEROMETER
	if( _debug )

		debugPrint( "Output data type: Accelerometer\nLast packet was: %5f, %5f, %5f\n", _acceleration_float_packet[0], _acceleration_float_packet[1], _acceleration_float_packet[2] );
#endif

#ifdef GET_DATA_GYROSCOPE
	if( _debug )

		debugPrint( "Output data type: Gyroscope\nLast packet was: %5f, %5f, %5f\n", _gyroscope_float_packet[0], _gyroscope_float_packet[1], _gyroscope_float_packet[2] );
#endif

#ifdef GET_DATA_YAWPITCHROLL
	if( _debug )

		debugPrint( "Output data type: Yaw, Pitch and Roll\nLast packet was: %5f, %5f, %5f\n", _yaw_pitch_roll_packet[0], _yaw_pitch_roll_packet[1], _yaw_pitch_roll_packet[2] );
#endif
}
/**
 * @brief Obtain the data from the sensor. Collects the FIFO packet and extracts the needed data.
 *
 */
void ActionTracer::TracePoint::get_data() {
	this->_select_me();

	if( !_dmp_ready ) {
		if( _debug )
			debugPrint( "DMP not initialised\n" );
		this->_deselect_me();
		return;
	}

	_device_interrupt_status = _device->getIntStatus();

	// does the FIFO have data in it?
	if( ( _device_interrupt_status & 0x02 ) < 1 ) {
		if( _debug )
			debugPrint( "Data not ready" );
		this->_deselect_me();
		return;
	}

	_fifo_count = _device->getFIFOCount();

	if( _fifo_count < _packet_size ) {
		if( _debug )
			debugPrintln( "MPU interrupt not ready or not enough elements in FIFO\n" );
		this->_deselect_me();
		return;
	}

	if( _fifo_count == 1024 ) {
		// reset so we can continue cleanly
		_device->resetFIFO();
		if( _debug )
			debugPrint( "FIFO overflow!\n" );
		return;
	}

	_device->getFIFOBytes( _fifo_buffer, _packet_size );

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
	// _device->resetFIFO();
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
 * @brief Returns the given device name
 *
 * @return std::string containing the device name
 */
std::string ActionTracer::TracePoint::get_name() {
	return _device_name;
}

/**
 * @brief Set debug printing on or off.
 *
 * @param value true or false
 */
void ActionTracer::TracePoint::set_debug( bool value ) {
	_debug = value;
}

/**
 * @brief Sets device offsets based on a pointers.csv file. This file should contain the callibration details of each device. Such a csv can be obtained by running the IMU_Zero program in the Offsts directory
 * @return Nothing
 */
void ActionTracer::TracePoint::_set_device_offsets() {
	// Check if the mapping file exists. If not, set random defaults.
	bool set = false;

	std::ifstream infile( "pointers.csv" );
	if( !infile.good() ) {
		_set_default_device_offsets();
	} else {
		// If file exists, use it to set offsets.
		std::string	  line;
		std::ifstream myfile( "pointers.csv" );

		while( std::getline( myfile, line ) ) {
			std::stringstream point_line( line );
			std::string		  value;
			size_t			  count = 0;

			while( getline( point_line, value, ',' ) ) {
				// First check if the needed pin exists. If not, set defaults
				int tp_value = atoi( value.c_str() );
				if( count == 0 ) {
					if( tp_value != _pin_number ) {
						break;
					} else {
						debugPrint( "Pin %d found in pointers.csv. Its parameters are:\n%s\n", _pin_number, line.c_str() );
						set = true;
					}
				}

				switch( count ) {
					case 1:
						_device->setXAccelOffset( tp_value );
						break;
					case 2:
						_device->setYAccelOffset( tp_value );
						break;
					case 3:
						_device->setZAccelOffset( tp_value );
						break;
					case 4:
						_device->setXGyroOffset( tp_value );
						break;
					case 5:
						_device->setYGyroOffset( tp_value );
						break;
					case 6:
						_device->setZGyroOffset( tp_value );
						break;

					default:
						break;
				}
				++count;
			}
		}
	}

	if( set != true ) {
		_set_default_device_offsets();
	}
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
