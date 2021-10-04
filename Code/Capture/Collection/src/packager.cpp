#include "debug_printer.h"
#include "packager.h"

using namespace ActionTracer;

ActionTracer::Packager::Packager( std::string destination, int port ) {
	_dest  = destination;
	_port  = port;
	_count = 0;
	struct sockaddr_in server;
	// Create socket
	_descriptor = socket( AF_INET, SOCK_DGRAM, 0 );
	if( _descriptor == -1 )
		debugPrint( "Could not create socket" );

	server.sin_addr.s_addr = inet_addr( _dest.c_str() ); // Destination address
	server.sin_family	   = AF_INET;
	server.sin_port		   = htons( _port ); // Destination port

	// Connect to remote server
	if( connect( _descriptor, ( struct sockaddr * ) &server, sizeof( server ) ) < 0 ) {
		debugPrint( "connect error" );
	} else {
		debugPrint( "Connected\n" );
	}
}

ActionTracer::Packager::~Packager() { 1; }

std::string ActionTracer::Packager::_float_to_string( float value ) {
	std::stringstream stream;
	stream << std::fixed << std::setprecision( 6 ) << value;
	std::string s = stream.str();

	return s;
}

int ActionTracer::Packager::send_packet( float *data, uint8_t length = 4 ) {
	std::string arr = "";

	// Convert floats to string
	for( int i = 0; i < length; i++ ) {
		arr += _float_to_string( data[i] );
		if( i != length - 1 ) {
			arr += ", ";
		}
	}

	debugPrint( arr.c_str() );
	// Send some data
	if( send( _descriptor, arr.c_str(), strlen( arr.c_str() ), 0 ) < 0 ) {
		debugPrint( "Send failed" );
		return 1;
	}
	debugPrint( "%7d - Sent", _count++ );
	return 0;
}