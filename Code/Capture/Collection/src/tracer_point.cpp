#include "MPU6050_6Axis_MotionApps20.h"
#include "debug_printer.h"
#include "tracer_point.h"

#include <wiringPi.h>

using namespace ActionTracer;

ActionTracer::TracePoint::TracePoint() {}

ActionTracer::TracePoint::TracePoint( MPU6050 *dev, std::string name, int wiring_Pi_pin_number, int output_data = 0 ) {
	debugPrintln( "Constructing the device as is needed. Name = %s\n", name.c_str() );

	_device			  = dev;
	_device_name	  = name;
	_pin_number		  = wiring_Pi_pin_number;
	_output_data_type = output_data;

	// Set pin information
	pinMode( _pin_number, OUTPUT );

	debugPrint( "Initilizing %s...\n", _device_name.c_str() );
	this->_select_me();
	_device->initialize();
	debugPrint( _device->testConnection() ? "%s connection successful\n" : "%s connection failed\n", _device_name.c_str() );

	// _device->setXAccelOffset( 33 );
	// _device->setYAccelOffset( 29 );
	// _device->setZAccelOffset( 61 );
	// _device->setXGyroOffset( -105 );
	// _device->setYGyroOffset( 1443 );
	// _device->setZGyroOffset( 4945 );

	_device->setXAccelOffset(44);
	_device->setYAccelOffset(25);
	_device->setZAccelOffset(74);
	_device->setXGyroOffset(-57);
	_device->setYGyroOffset(1449);
	_device->setZGyroOffset(4973);

	// DMP Initialization
	_dmp_status = _device->dmpInitialize();
	if( _dmp_status == 0 ) {
		debugPrint( "Enabling DMP..." );
		_device->setDMPEnabled( true );
		_dmp_ready = OK;
		debugPrint( "Enabled!\n" );
	} else {
		debugPrint( "Can't initialise DMP\n" );
		_dmp_ready = ERROR;
	}

	// Reset the FIFO
	_device->resetFIFO();
	_packet_size = _device->dmpGetFIFOPacketSize();

	this->_deselect_me();
}

// ActionTracer::TracePoint ActionTracer::TracePoint::operator=( const ActionTracer::TracePoint &other ) const {
// 	TracePoint result;

// 	result._device			 = _device;
// 	result._device_name		 = _device_name;
// 	result._pin_number		 = _pin_number;
// 	result._output_data_type = _output_data_type;

// 	return result;
// }

/** Selects a given MPU6050 node. Must be deselected to avoid issues.
 * @return 0 if all gows well.
*/
int ActionTracer::TracePoint::_select_me() {
	debugPrintln( "Selected %s\n", _device_name.c_str() );
	digitalWrite( _pin_number, LOW );
	return OK;
}

/** Deselects a given MPU6050 node.
 * @return 0 if all gows well.
*/
int ActionTracer::TracePoint::_deselect_me() {
	debugPrintln( "Deselected %s\n", _device_name.c_str() );
	digitalWrite( _pin_number, HIGH );
	return OK;
}

/** Calls on the selected sensor to identify itself by its given name.
 * @return Device name/id
*/
std::string ActionTracer::TracePoint::identify() {
	debugPrint( "I am %s\n", _device_name.c_str() );
	return _device_name;
}

void ActionTracer::TracePoint::print_last_data_packet() {
	switch( _output_data_type ) {
		case GET_DATA_QUATERNION:
			debugPrint( "Output data type: Quaternion\nLast packet was: %5f, %5f, %5f, %5f\n", _quaternion_float_packet[0], _quaternion_float_packet[1], _quaternion_float_packet[2], _quaternion_float_packet[3] );
			break;
		case GET_DATA_EULER:
			debugPrint( "Output data type: Euler\nLast packet was: %5f, %5f, %5f\n", _euler_packet[0], _euler_packet[1], _euler_packet[2] );
			break;
		case GET_DATA_ACCELEROMETER:
			debugPrint( "Output data type: Accelerometer\nLast packet was: %5f, %5f, %5f\n", _acceleration_float_packet[0], _acceleration_float_packet[1], _acceleration_float_packet[2] );
			break;
		case GET_DATA_GYROSCOPE:
			debugPrint( "Output data type: Gyroscope\nLast packet was: %5f, %5f, %5f\n", _gyroscope_float_packet[0], _gyroscope_float_packet[1], _gyroscope_float_packet[2] );
			break;
		case GET_DATA_YAWPITCHROLL:
			debugPrint( "Output data type: Yaw, Pitch and Roll\nLast packet was: %5f, %5f, %5f\n", _yaw_pitch_roll_packet[0], _yaw_pitch_roll_packet[1], _yaw_pitch_roll_packet[2] );
			break;
		default:
			this->identify();
			break;
	}
}

void ActionTracer::TracePoint::get_data() {
	this->_select_me();

	_device->getFIFOBytes( _fifo_buffer, _packet_size );

	if( _fifo_count >= 1024 ) {
		debugPrint( "%s: FIFO overflow!\n", _device_name.c_str() );
		_device->resetFIFO();
	}

	switch( _output_data_type ) {
		case GET_DATA_QUATERNION:
			_device->dmpGetQuaternion( &_quaternion_packet, _fifo_buffer );
			_quaternion_float_packet[0] = _quaternion_packet.w;
			_quaternion_float_packet[1] = _quaternion_packet.x;
			_quaternion_float_packet[2] = _quaternion_packet.y;
			_quaternion_float_packet[3] = _quaternion_packet.z;
			break;
		case GET_DATA_EULER:
			_device->dmpGetQuaternion( &_quaternion_packet, _fifo_buffer );
			_device->dmpGetEuler( &_euler_packet[0], &_quaternion_packet );
			break;
		case GET_DATA_ACCELEROMETER:
			_device->dmpGetAccel( &_acceleration_packet );
			_acceleration_float_packet[0] = _acceleration_packet.x;
			_acceleration_float_packet[1] = _acceleration_packet.y;
			_acceleration_float_packet[2] = _acceleration_packet.z;
			break;
		case GET_DATA_GYROSCOPE:
			_device->dmpGetGyro( &_gyroscope_packet );
			_gyroscope_float_packet[0] = _gyroscope_packet.x;
			_gyroscope_float_packet[1] = _gyroscope_packet.y;
			_gyroscope_float_packet[2] = _gyroscope_packet.z;
			break;
		case GET_DATA_YAWPITCHROLL:
			_device->dmpGetQuaternion( &_quaternion_packet, _fifo_buffer );
			_device->dmpGetGravity( &_gravity_packet, &_quaternion_packet );
			_device->dmpGetYawPitchRoll( &_yaw_pitch_roll_packet[0], &_quaternion_packet, &_gravity_packet );
			break;
		default:
			_device->dmpGetQuaternion( &_quaternion_packet, _fifo_buffer );
			_quaternion_float_packet[0] = _quaternion_packet.w;
			_quaternion_float_packet[1] = _quaternion_packet.x;
			_quaternion_float_packet[2] = _quaternion_packet.y;
			_quaternion_float_packet[3] = _quaternion_packet.z;
			break;
	}

	debugPrint( "Data fetched\n" );

	this->_deselect_me();
}

float *ActionTracer::TracePoint::read_data( int read_first = 0 ) {
	if( read_first ) {
		this->get_data();
		debugPrint( "Obtained data from %s:\n", _device_name.c_str() );
	}

	switch( _output_data_type ) {
		case GET_DATA_QUATERNION:
			debugPrint( "\tFetched: x: %4.2f, y: %4.2f, z: %4.2f, w: %4.2f\n", _quaternion_packet.x, _quaternion_packet.y, _quaternion_packet.z, _quaternion_packet.w );
			return _quaternion_float_packet;
		case GET_DATA_EULER:
			return _euler_packet;
		case GET_DATA_ACCELEROMETER:
			return _acceleration_float_packet;
		case GET_DATA_GYROSCOPE:
			return _gyroscope_float_packet;
		case GET_DATA_YAWPITCHROLL:
			return _yaw_pitch_roll_packet;
		default:
			debugPrint( "Fetched: x: %4.2f, y: %4.2f, z: %4.2f, w: %4.2f", _quaternion_packet.x, _quaternion_packet.y, _quaternion_packet.z, _quaternion_packet.w );
			return _quaternion_float_packet;
	}
}

void ActionTracer::TracePoint::set_output_data_type( int data_type ) {
	switch( _output_data_type ) {
		case GET_DATA_QUATERNION:
			debugPrint( "Name: %s\tOutput data type: Quaternion\n", _device_name.c_str() );
			break;
		case GET_DATA_EULER:
			debugPrint( "Name: %s\tOutput data type: Euler\n", _device_name.c_str() );
			break;
		case GET_DATA_ACCELEROMETER:
			debugPrint( "Name: %s\tOutput data type: Accelerometer\n", _device_name.c_str() );
			break;
		case GET_DATA_GYROSCOPE:
			debugPrint( "Name: %s\tOutput data type: Gyroscope\n", _device_name.c_str() );
			break;
		case GET_DATA_YAWPITCHROLL:
			debugPrint( "Name: %s\tOutput data type: Yaw, Pitch and Roll\n", _device_name.c_str() );
			break;
		default:
			this->identify();
			break;
	}
	_output_data_type = data_type;
}
