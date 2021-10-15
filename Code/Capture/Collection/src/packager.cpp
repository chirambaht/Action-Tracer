#include "debug_printer.h"
#include "packager.h"

#include <ctime>

using namespace ActionTracer;

ActionTracer::Packager::Packager( std::string destination, int port ) {
	_dest  = destination;
	_port  = port;
	_count = 0;

	debugPrint( "Creating network socket via UDP on port %d, to IP:%s...\n", _port, _dest.c_str() );
	struct sockaddr_in server;
	// Create socket
	_descriptor = socket( AF_INET, SOCK_DGRAM, 0 );
	if( _descriptor == -1 )
		debugPrint( "Could not create socket\n" );

	server.sin_addr.s_addr = inet_addr( _dest.c_str() ); // Destination address
	server.sin_family	   = AF_INET;
	server.sin_port		   = htons( _port ); // Destination port

	// Connect to remote server
	if( connect( _descriptor, ( struct sockaddr * ) &server, sizeof( server ) ) < 0 ) {
		debugPrint( "connect error\n" );
	} else {
		debugPrint( "UDP client ready!\n" );
	}
}

// ActionTracer::Packager::~Packager() {}

std::string ActionTracer::Packager::_float_to_string( float value, int prec = 6 ) {
	std::stringstream stream;
	stream << std::fixed << std::setprecision( prec ) << value;
	std::string s = stream.str();

	return s;
}

int ActionTracer::Packager::send_packet( float *data, uint8_t length = 4 ) {
	std::string arr = "";

#if DEBUG == 1
	std::string db_arr = "";
	for( int i = 0; i < length; i++ ) {
		db_arr += _float_to_string( data[i], 2 );
		if( i != length - 1 ) {
			db_arr += ", ";
		}
	}
	debugPrint( "%7d - %s:%d\n%s\n\n", _count, _dest.c_str(), _port, db_arr.c_str() );
#endif
	// Convert floats to string
	for( int i = 0; i < length; i++ ) {
		arr += _float_to_string( data[i], 6 );
		if( i != length - 1 ) {
			arr += ", ";
		}
	}

	// Send some data
	if( send( _descriptor, arr.c_str(), strlen( arr.c_str() ), 0 ) < 0 ) {
		debugPrint( "Send failed" );
		logger.open();
		_log( "Send failed" );
		return 1;
	}
	_count++;
	_log( arr.c_str() );

	return 0;
}

void ActionTracer::Packager::_log( std::string data ) {
	std::time_t t	= std::time( 0 ); // get time now
	std::tm *	now = std::localtime( &t );
	_logger			= fopen( "packager.log", "w" );
	fprintf( _logger, "%4d-%2d-%2d @  %2d:%2d:%2d-> %s\n", now->tm_year + 1900, ( now->tm_mon + 1 ), now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, data.c_str() );
	fclose( _logger );
}