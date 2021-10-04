#include "tracer_point.h"

using namespace ActionTracer;

ActionTracer::TracePoint::TracePoint( MPU6050 *dev, std::string name, int wiring_Pi_pin_number, int output_data = 0 ) {
	debugPrintln( "Constructing the device as is needed. Name = %s\n", name );

	_device			  = dev;
	_device_name	  = name;
	_pin_number		  = wiring_Pi_pin_number;
	_output_data_type = output_data;

		// Set pin information
		pinMode( _pin_number, OUTPUT );

	debugPrint( "Initilizing %s...\n", _device_name );
	_select_me();
	_device->initialize();
	debugPrint( _device.testConnection() ? "%s connection successful\n" : "%s connection failed\n", _device_name );

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
	_device->resetFIFO;
	_device->dmpGetFIFOPacketSize();

	_device._deselect_me();
}

/** Selects a given MPU6050 node. Must be deselected to avoid issues.
 * @return 0 if all gows well.
*/
int ActionTracer::TracePoint::_select_me() {
	debugPrintln( "Selected %s\n", _device_name );
	digitalWrite( _pin_number, LOW );
	return OK;
}

/** Deselects a given MPU6050 node.
 * @return 0 if all gows well.
*/
int ActionTracer::TracePoint::_deselect_me() {
	debugPrintln( "Deselected %s\n", _device_name );
	digitalWrite( _pin_number, LOW );
	return OK;
}

/** Calls on the selected sensor to identify itself by its given name.
 * @return Device name/id
*/
std::string ActionTracer::TracePoint::identify() {
	debugPrint( "I am %s\n", _device_name );
	return _device_name;
}

void ActionTracer::TracePoint::print_last_data_packet() {
	switch( _output_data_type ) {
		case GET_DATA_QUATERNION:
			debugPrint( "Output data type: Quaternion\nLast packet was: %5f, %5f, %5f, %5f\n", _device_name, _quaternion_packet[0], _quaternion_packet[1], _quaternion_packet[2], _quaternion_packet[3] );
			break;
		case GET_DATA_EULER:
			debugPrint( "Output data type: Euler\nLast packet was: %5f, %5f, %5f\n", _device_name, _euler_packet[0], _euler_packet[1], _euler_packet[2] );
			break;
		case GET_DATA_ACCELEROMETER:
			debugPrint( "Output data type: Accelerometer\nLast packet was: %5f, %5f, %5f\n", _device_name, _acceleration_packet[0], _acceleration_packet[1], _acceleration_packet[2] );
			break;
		case GET_DATA_GYROSCOPE:
			debugPrint( "Output data type: Gyroscope\nLast packet was: %5f, %5f, %5f\n", _device_name, _gyroscope_packet[0], _gyroscope_packet[1], _gyroscope_packet[2] );
			break;
		case GET_DATA_YAWPITCHROLL:
			debugPrint( "Output data type: Yaw, Pitch and Roll\nLast packet was: %5f, %5f, %5f\n", _device_name, _yaw_pitch_roll_packet[0], _yaw_pitch_roll_packet[1], _yaw_pitch_roll_packet[2] );
			break;
		default:
			this->identify();
			break;
	}
}

void ActionTracer::TracePoint::get_data() {
	_select_me();

	_fifo_count = _device->getFIFOBytes( _fifo_buffer, _packet_size );

	if( _fifo_count >= 1024 ) {
		debugPrint( "%s: FIFO overflow!\n", _device_name );
		_device->resetFIFO();
	}

	switch( _output_data_type ) {
		case GET_DATA_QUATERNION:
			_device->dmpGetQuaternion( &_quaternion_packet, _fifo_buffer );
			break;
		case GET_DATA_EULER:
			_device->dmpGetEuler( &_euler_packet, _fifo_buffer );
			break;
		case GET_DATA_ACCELEROMETER:
			_device->dmpGetAccel( &_acceleration_packet );
			break;
		case GET_DATA_GYROSCOPE:
			_device->dmpGetGyro( &_acceleration_packet );
			break;
		case GET_DATA_YAWPITCHROLL:
			_device->dmpGetQuaternion( &_quaternion_packet, _fifo_buffer );
			_device->dmpGetGravity( &_gravity_packet, _fifo_buffer );
			_device->dmpGetYawPitchRoll( &_yaw_pitch_roll_packet, &_quaternion_packet, &_gravity_packet );
			break;
		default:
			break;
	}

	_deselect_me();
}

float *ActionTracer::TracePoint::read_data( int read_first = 0 ) {
	if( read_first ) {
		this->get_data();
	}

	switch( _output_data_type ) {
		case GET_DATA_QUATERNION:
			return _quaternion_packet;
		case GET_DATA_EULER:
			return _euler_packet;
		case GET_DATA_ACCELEROMETER:
			return _acceleration_packet;
		case GET_DATA_GYROSCOPE:
			return _gyroscope_packet;
		case GET_DATA_YAWPITCHROLL:
			return _yaw_pitch_roll_packet;
		default:
			return _quaternion_packet;
	}
}

void ActionTracer::TracePoint::set_output_data_type( int data_type ) {
	switch( _output_data_type ) {
		case GET_DATA_QUATERNION:
			debugPrint( "Name: %s\tOutput data type: Quaternion\n", _device_name );
			break;
		case GET_DATA_EULER:
			debugPrint( "Name: %s\tOutput data type: Euler\n", _device_name );
			break;
		case GET_DATA_ACCELEROMETER:
			debugPrint( "Name: %s\tOutput data type: Accelerometer\n", _device_name );
			break;
		case GET_DATA_GYROSCOPE:
			debugPrint( "Name: %s\tOutput data type: Gyroscope\n", _device_name );
			break;
		case GET_DATA_YAWPITCHROLL:
			debugPrint( "Output data type: Yaw, Pitch and Roll\n", _device_name );
			break;
		default:
			this->identify();
			break;
	}
	_output_data_type = data_type;
}
