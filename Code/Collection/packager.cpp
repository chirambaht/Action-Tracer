#define debugPrint( ... )	printf( __VA_ARGS__ )
#define debugPrintln( ... ) printf( __VA_ARGS__ )

#include "packager.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

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

	// if( bind( _descriptor, ( struct sockaddr * ) &_server, sizeof( _server ) ) < 0 ) {
	// 	perror( "bind failed" );
	// 	exit( EXIT_FAILURE );
	// }

	if( connect( _descriptor, ( struct sockaddr * ) &_server, sizeof( _server ) ) < 0 ) {
		perror( "connection failed" );
		exit( EXIT_FAILURE );
	}

	if( _debug )
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
		if( _debug )
			debugPrint( "connect error\n" );
	} else {
		if( _debug )
			debugPrint( "UDP client ready!\n" );
	}

	if( _debug )
		debugPrint( "Creating network socket via UDP on port %d, to IP:%s...\n", _port, _dest.c_str() );
}

/**
 * @brief Converts a given float value to a string to a specified number of decimal places.
 *
 * @param value float to be converted to string
 * @param prec number of decimal places. Defaults to 6
 * @return std::string representation of given float
 */
std::string
ActionTracer::Packager::_float_to_string( float value, int prec = 6 ) {
	std::stringstream stream;
	if( value < 0 ) {
		prec -= 1;
	}
	stream << std::fixed << std::setprecision( prec ) << value;
	std::string s = stream.str();

	return s;
}

/**
 * @brief Set debug printing on or off.
 *
 * @param value true or false
 */
void ActionTracer::Packager::set_debug( bool value ) {
	_debug = value;
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
	if( send( _descriptor, _package.c_str(), strlen( _package.c_str() ), 0 ) < 0 ) {
		if( _debug )
			debugPrint( "Send failed\n" );
		// return 1;
	}

#ifdef ON_PI
	if( _save )
		fprintf( _recording, "%8d,%7d,%s\n", millis() - _recording_start_time, _count, _package.c_str() );
#endif

	if( _debug ) {
		// debugPrint( "\x1B[2J" );
		debugPrint( "%7d - %s:%d ==> %s\n", _count, _dest.c_str(), _port, _package.c_str() );
	}

	_count++;
	_package = "";

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
		_package += _float_to_string( data[i], 6 );
		if( i != length - 1 ) {
			_package += ",";
		}
	}
	_package += ":";
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
 * @brief Opens the recording file
 */
void ActionTracer::Packager::close_file() {
	fclose( _recording );
}

/**
 * @brief Closes the recording file.
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