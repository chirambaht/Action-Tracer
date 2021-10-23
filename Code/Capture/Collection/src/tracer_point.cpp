#include "tracer_point.h"

#include "MPU6050_6Axis_MotionApps20.h"
#include "debug_printer.h"

#include <wiringPi.h>

using namespace ActionTracer;

ActionTracer::TracePoint::TracePoint() {}

ActionTracer::TracePoint::TracePoint( std::string name, int wiring_Pi_pin_number, int output_data = 0 ) {
	debugPrintln( "Constructing the device as is needed. Name = %s\n", name.c_str() );

	_device_name	  = name;
	_pin_number		  = wiring_Pi_pin_number;
	_output_data_type = output_data;

	// Set pin information
	pinMode( _pin_number, OUTPUT );
	_device_interrupt_flag = false;

	debugPrint( "Initilizing %s...\n", _device_name.c_str() );
	this->_select_me();
	_device = new MPU6050( MPU6050_ADDRESS_AD0_LOW );

	_device->initialize();

	//TODO: At this stage an interrupt pin is initialised

	debugPrint( _device->testConnection() ? "%s connection successful\n" : "%s connection failed\n", _device_name.c_str() );

	// DMP Initialization
	debugPrint( "Initalising DMP\n" );
	_device_status = _device->dmpInitialize();

	// Set up the offsets for the device
	_device->setXAccelOffset( 44 );
	_device->setYAccelOffset( 25 );
	_device->setZAccelOffset( 74 );
	_device->setXGyroOffset( -57 );
	_device->setYGyroOffset( 1449 );
	_device->setZGyroOffset( 4973 );

	if( _device_status == 0 ) {
		debugPrint( "Enabling DMP..." );
		_device->setDMPEnabled( true );

		//TODO: Attach interrupt here

		_device_interrupt_status = _device->getIntStatus();

		_dmp_ready = 1;

		_packet_size = _device->dmpGetFIFOPacketSize();

		debugPrint( "Enabled!\n" );
	} else {
		debugPrint( "Can't initialise DMP\n" );
		_dmp_ready = 0;
	}
	this->_deselect_me();

#if DEBUG == 1
	debugPrint( "Init variable dump\n" );
	debugPrint( "\n\tDevice Name:\t\t%s\n\tPin number:\t\t%d\n\tOutput data:\t\t%d\n\tDMP Status:\t\t%d\n\tFIFO Packet Size:\t%d\n", _device_name.c_str(), _pin_number, output_data, _dmp_ready, _packet_size );
#endif
}

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

	if( !_dmp_ready ) {
		debugPrint( "DMP not initialised\n" );
		return;
	}

	if( _device_interrupt_flag && _fifo_count < _packet_size ) {
		debugPrintln( "MPU interrupt not ready or not enough elements in FIFO\n" );
		return;
	}

	_device_interrupt_flag	 = false;
	_device_interrupt_status = _device->getIntStatus();

	_fifo_count = _device->getFIFOCount();

	if( _device_interrupt_status & 0x10 || _fifo_count == 1024 ) {
		_device->resetFIFO();
		debugPrintln( "FIFO Overload (Inefficeint code)\n" );
	} else if( _device_interrupt_status & 0x02 ) {
		// Wait for the proper data length
		while( _fifo_count < _packet_size ) {
			_fifo_count = _device->getFIFOCount();
		}

		// Read FIFO packet
		_device->getFIFOBytes( _fifo_buffer, _packet_size );

		// Track the FIFO (for more than one packet) and also allows reading without interrupt
		_fifo_count -= _packet_size;

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
