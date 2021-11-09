#define debugPrint( ... )	printf( __VA_ARGS__ )
#define debugPrintln( ... ) printf( __VA_ARGS__ )

#include "packager.h"

#include <ctime>

using namespace ActionTracer;

ActionTracer::Packager::Packager( std::string destination, int port ) {
	_dest  = destination;
	_port  = port;
	_count = 0;

	if( _debug )
		debugPrint( "Creating network socket via UDP on port %d, to IP:%s...\n", _port, _dest.c_str() );
	struct sockaddr_in server;
	// Create socket
	_descriptor = socket( AF_INET, SOCK_DGRAM, 0 );
	if( _descriptor == -1 )
		if( _debug )
			debugPrint( "Could not create socket\n" );

	server.sin_addr.s_addr = inet_addr( _dest.c_str() ); // Destination address
	server.sin_family	   = AF_INET;
	server.sin_port		   = htons( _port ); // Destination port

	// Connect to remote server
	if( connect( _descriptor, ( struct sockaddr * ) &server, sizeof( server ) ) < 0 ) {
		if( _debug )
			debugPrint( "connect error\n" );
	} else {
		if( _debug )
			debugPrint( "UDP client ready!\n" );
	}
}

// ActionTracer::Packager::~Packager() {}

std::string ActionTracer::Packager::_float_to_string( float value, int prec = 6 ) {
	std::stringstream stream;
	if( value < 0 ) {
		prec -= 1;
	}
	stream << std::fixed << std::setprecision( prec ) << value;
	std::string s = stream.str();

	return s;
}

void ActionTracer::Packager::set_debug( bool value ) {
	_debug = value;
}

/**
 * This is used to send the stored data packet in @code _packet @endcode
 * @return 0 if succesful.
*/
int ActionTracer::Packager::send_packet() {
	// Send some data
	if( send( _descriptor, _package.c_str(), strlen( _package.c_str() ), 0 ) < 0 ) {
		if( _debug )
			debugPrint( "Send failed\n" );
		return 1;
	}
	if( _debug )
		debugPrint( "%7d - %s:%d ==> %s\n", _count, _dest.c_str(), _port, _package.c_str() );
	_count++;
	_package = "";

	return 0;
}

/**
 * This is used to add data to a package that is going to be sent. It takes an array of floats.
 * @param *data A reference to an array of floats
 * @param length number of floats in array to convert. Defaults to 4
 * @return 0 if succesful.
*/
int ActionTracer::Packager::load_packet( float *data, uint8_t length = 4 ) {
	for( int i = 0; i < length; i++ ) {
		_package += _float_to_string( data[i], 6 );
		if( i != length - 1 ) {
			_package += ", ";
		}
	}
	return 0;
}
