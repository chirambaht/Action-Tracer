#ifdef DEBUG
	#define debugPrint( ... )	printf( __VA_ARGS__ )
	#define debugPrintln( ... ) printf( __VA_ARGS__ )
#else
	#define debugPrint( ... )
	#define debugPrintln( ... )
#endif

#include "packager.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unistd.h>

#ifdef ON_PI
	#include <wiringPi.h>
#endif

using namespace ActionTracer;

/**
 * @brief Construct a new Action Tracer:: Packager:: Packager object
 *
 * @param destination IP Address to send data to
 * @param port Destination UDP Port to send data to
 */
ActionTracer::Packager::Packager( std::string destination, int port ) {
	_dest  = destination;
	_port  = port;
	_count = 0;

	// if the save method is enabled, we need to store the data we are sending to a file.
	if( _save ) {
		this->open_file();
	}
}

/**
 * @brief Initialize a TCP client on the device
 *
 */
void ActionTracer::Packager::init_tcp() {
	_descriptor = socket( AF_INET, SOCK_STREAM, 0 );
	int _opt	= 1;
	if( _descriptor == 0 ) {
		debugPrint( "Error" );
	}

	// This helps in manipulating options for the socket referred by the file descriptor sockfd. This is completely optional, but it helps in reuse of address and port. Prevents error such as: “address already in use”.
	if( setsockopt( _descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_opt, sizeof( _opt ) ) ) {
		perror( "setsockopt" );
		exit( EXIT_FAILURE );
	}

	_server.sin_addr.s_addr = inet_addr( _dest.c_str() ); // Destination address
	_server.sin_family		= AF_INET;
	_server.sin_port		= htons( _port ); // Destination port

	if( connect( _descriptor, ( struct sockaddr * ) &_server, sizeof( _server ) ) < 0 ) {
		perror( "connection failed" );
		exit( EXIT_FAILURE );
	}

	debugPrint( "Creating network socket via TCP on port %d, to IP:%s...\n", _port, _dest.c_str() );
}

/**
 * @brief Initialize a UDP client on the device
 *
 */
void ActionTracer::Packager::init_udp() {
	_descriptor = socket( AF_INET, SOCK_DGRAM, 0 );
	if( _descriptor == 0 ) {
		debugPrint( "Error" );
	}

	_server.sin_addr.s_addr = inet_addr( _dest.c_str() ); // Destination address
	_server.sin_family		= AF_INET;
	_server.sin_port		= htons( _port ); // Destination port

	// Connect to remote server
	if( connect( _descriptor, ( struct sockaddr * ) &_server, sizeof( _server ) ) < 0 ) {
		debugPrint( "connect error\n" );
	} else {
		debugPrint( "UDP client ready!\n" );
	}

	debugPrint( "Creating network socket via UDP on port %d, to IP:%s...\n", _port, _dest.c_str() );
}

/**
 * @brief Converts a given float value to a string to a specified number of decimal places.
 *
 * @param value float to be converted to string
 * @param prec number of decimal places. Defaults to 6
 * @return std::string representation of given float
 */
std::string ActionTracer::Packager::_float_to_string( float value, int prec = 6 ) {
	std::stringstream stream;
	if( value < 0 ) {
		prec -= 1;
	}
	stream << std::fixed << std::setprecision( prec ) << value;
	std::string s = stream.str();

	return s;
}

/**
 * @brief Converts a given float value to a integer to 3 decimal places.
 *
 * @param value float to be converted to integer
 * @return 16 bit integer of the inital value
 */
__int16_t ActionTracer::Packager::_float_to_int( float value ) {
	return static_cast<__int16_t>( value * 100000 );
}

/**
 * This is used to send the stored data packet in @code _packet @endcode
 * @return 0 if successful.
 */
int ActionTracer::Packager::send_packet() {
	std::thread offloaded_send( &ActionTracer::Packager::_send_packet, this );
	offloaded_send.detach();
	return 0;
}

/**
 * This is used to send the stored data packet in @code _packet @endcode
 * @return 0 if successful.
 */
void ActionTracer::Packager::_send_packet() {
// Send some data
#ifdef SEND_INT
	// When this data is sent, it will be sent a single array element at a time. each element is 2 bytes (16 bits) but they are sent in reverse order i.e. TP captures 0x23ef but packager will send it as  0xef23.
	if( send( _descriptor, _package, sizeof( _package ), 0 ) < 0 ) {
		debugPrint( "Send failed\n" );
	}
#else
	if( send( _descriptor, _package.c_str(), strlen( _package.c_str() ), 0 ) < 0 ) {
		debugPrint( "Send failed\n" );
	}
#endif

#ifdef ON_PI
	if( _save ) {
	#ifdef SEND_INT
		fprintf( _recording, "%8d,%7d,%3i,%3i,%3i,%3i:%3i,%3i,%3i,%3i:%3i,%3i,%3i,%3i\n", millis() - _recording_start_time, _count, _package[0], _package[1], _package[2], _package[3], _package[4], _package[5], _package[6], _package[7], _package[8], _package[9], _package[10], _package[11] );
	#else
		fprintf( _recording, "%8i,%7i,%s\n", millis() - _recording_start_time, _count, _package.c_str() );
	#endif
	}
#endif

	// debugPrint( "\x1B[2J" );
#ifdef SEND_INT
	debugPrint( "%8d,%7d,%3i,%3i,%3i,%3i:%3i,%3i,%3i,%3i:%3i,%3i,%3i,%3i\n", millis() - _recording_start_time, _count, _package[0], _package[1], _package[2], _package[3], _package[4], _package[5], _package[6], _package[7], _package[8], _package[9], _package[10], _package[11] );
#else
	debugPrint( "%8i,%7i,%s\n", millis() - _recording_start_time, _count, _package.c_str() );
#endif

	_count++;
#ifdef SEND_INT
	for( size_t r = 0; r < _package_packets; r++ ) {
		_package[r] = 0;
	}
#else
	_package = "";

#endif

	// return 0;
}

/**
 * This is used to add data to a package that is going to be sent. It takes an array of floats.
 * @param *data A reference to an array of floats
 * @param length number of floats in array to convert. Defaults to 4
 * @return 0 if successful.
 */
int ActionTracer::Packager::load_packet( float *data, uint8_t length = 4 ) {
	for( int i = 0; i < length; i++ ) {
#ifdef SEND_INT
		_package[_package_pointer++] = _float_to_int( data[i] );
		if( _package_pointer >= length * _number_of_devices ) {
			_package_pointer = 0;
		}
#else
		_package += _float_to_string( data[i], 6 );
		if( i != length - 1 ) {
			_package += ",";
		}
#endif
	}

#ifndef SEND_INT
	_package += ":";
#endif

	return 0;
}

/**
 * @brief Set file saving on or off.
 *
 * @param value true or false
 */
void ActionTracer::Packager::save_enable( bool value ) {
	_save = value;
	this->open_file();
}

/**
 * @brief Get the status of the saving
 *
 * @param value true or false
 */
bool ActionTracer::Packager::save_status() {
	return _save;
}

/**
 * @brief Closes the recording file
 */
void ActionTracer::Packager::close_file() {
	fclose( _recording );
}

/**
 * @brief Opens the recording file.
 */
void ActionTracer::Packager::open_file() {
	auto t	= std::time( nullptr );
	auto tm = *std::localtime( &t );

	std::ostringstream oss;
	oss << std::put_time( &tm, "%Y%m%d-%H%M%S" );
	auto str = oss.str();
	str += ".act";
	_recording = fopen( str.c_str(), "w" );
#ifdef ON_PI
	_recording_start_time = millis();
#endif
}

/**
 * @brief Closes the socket connection.
 */
void ActionTracer::Packager::close_socket() {
	close( _descriptor );
}