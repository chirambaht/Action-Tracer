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
	debugPrint( "Creating network socket via TCP on port %d, to IP:%s...\n", _port, _dest.c_str() );
	int d = socket( AF_INET, SOCK_STREAM, 0 );

	set_descriptor( d );

	if( _descriptor == -1 ) {
		debugPrint( "Error: %s\n", strerror( errno ) );
	}

	int _opt = 1;

	// This helps in manipulating options for the socket referred by the file descriptor sockfd. This is completely optional, but it helps in reuse of address and port. Prevents error such as: “address already in use”.
	if( setsockopt( _descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_opt, sizeof( _opt ) ) ) {
		perror( "setsockopt" );
		exit( EXIT_FAILURE );
	}

	_server.sin_addr.s_addr = inet_addr( _dest.c_str() ); // Destination address
	_server.sin_family		= AF_INET;
	_server.sin_port		= htons( _port ); // Destination port

	int con_res = connect( _descriptor, ( struct sockaddr * ) &_server, sizeof( _server ) );

	if( con_res < 0 ) {
		perror( "connection failed" );
		exit( EXIT_FAILURE );
	}
	debugPrint( "TCP socket connected with descriptor: %d\n", _descriptor );
}

/**
 * @brief Setup device as TCP server
 *
 * @return int File descriptor of the socket
 */

int ActionTracer::Packager::socket_setup() {
	if( ( _descriptor = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
		perror( "socket failed" );
		exit( EXIT_FAILURE );
	}
	debugPrint( "TCP socket created with descriptor: %d\n", _descriptor );
	int _opt = 1;

	// This helps in manipulating options for the socket referred by the file descriptor sockfd. This is completely optional, but it helps in reuse of address and port. Prevents error such as: “address already in use”.
	if( setsockopt( _descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_opt, sizeof( _opt ) ) ) {
		perror( "setsockopt" );
		exit( EXIT_FAILURE );
	}

	_server.sin_addr.s_addr = INADDR_ANY;
	_server.sin_family		= AF_INET;
	_server.sin_port		= htons( _port ); // My open port

	if( bind( _descriptor, ( struct sockaddr * ) &_server, sizeof( _server ) ) < 0 ) {
		perror( "bind failed" );
		exit( EXIT_FAILURE );
	}
	if( listen( _descriptor, ( MAX_CLIENTS / 2 ) ) == 1 ) {
		perror( "Error when trying to listen" );
		exit( EXIT_FAILURE );
	}

	_timeout.tv_sec	 = 0;
	_timeout.tv_usec = 500; // 0.5ms

	return _descriptor;
}

void ActionTracer::Packager::run_socket_manager() {
	while( true ) {
		FD_ZERO( &_readfds );
		FD_SET( _descriptor, &_readfds );
		max_sd = _descriptor;

		for( i = 0; i < MAX_CLIENTS; i++ ) {
			sd = _client_sockets[i];
			if( sd > 0 ) {
				FD_SET( sd, &_readfds );
			}
			if( sd > max_sd ) {
				max_sd = sd;
			}
		}

		activity = select( max_sd + 1, &_readfds, NULL, NULL, &_timeout );
		printf( "fd: %d\n", _descriptor );

		if( ( activity < 0 ) && ( errno != EINTR ) ) {
			printf( "select error\n" );
		}

		if( FD_ISSET( _descriptor, &_readfds ) ) {
			if( ( new_socket = accept( _descriptor, ( struct sockaddr * ) &_server, ( socklen_t * ) sizeof( _server ) ) ) < 0 ) {
				perror( "accept\n" );
				exit( EXIT_FAILURE );
			}
		}

		printf( "New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket, inet_ntoa( _server.sin_addr ), ntohs( _server.sin_port ) );

		for( i = 0; i < MAX_CLIENTS; i++ ) {
			if( _client_sockets[i] == 0 ) {
				_client_sockets[i] = new_socket;
				break;
			}
		}

		// Normal work resumes here
		for( i = 0; i < MAX_CLIENTS; i++ ) {
			sd = _client_sockets[i];

			if( FD_ISSET( sd, &_readfds ) ) {
				// Check if it was for closing , and also read the
				// incoming message
				if( ( valread = read( sd, _buffer, 1024 ) ) == 0 ) {
					// Somebody disconnected , get his details and print
					getpeername( sd, ( struct sockaddr * ) &_server, ( socklen_t * ) sizeof( _server ) );
					printf( "Host disconnected , ip %s , port %d \n",
						inet_ntoa( _server.sin_addr ), ntohs( _server.sin_port ) );
					close( sd );
					_client_sockets[i] = 0;
				}

				// Echo back the message that came in
				else {
					_send_packet( sd );
				}
			}
		}
	}
}

/**
 * @brief Initialize a UDP client on the device
 *
 */
void ActionTracer::Packager::init_udp() {
	debugPrint( "Creating network socket via UDP on port %d, to IP:%s...\n", _port, _dest.c_str() );
	_server.sin_addr.s_addr = inet_addr( _dest.c_str() ); // Destination address
	_server.sin_family		= AF_INET;
	_server.sin_port		= htons( _port ); // Destination port

	int d = socket( AF_INET, SOCK_DGRAM, 0 );

	set_descriptor( d );

	if( _descriptor == -1 ) {
		debugPrint( "Error: %s\n", strerror( errno ) );
	}

	// Connect to remote server
	int con_res = connect( _descriptor, ( struct sockaddr * ) &_server, sizeof( _server ) );
	if( ( con_res ) < 0 ) {
		debugPrint( "Error: %s\n", strerror( errno ) );
	} else {
		debugPrint( "UDP client ready!\n" );
	}
	debugPrint( "Connected\n" );
}

/**
 * @brief Converts a given float value to a integer to 3 decimal places.
 *
 * @param value float to be converted to integer
 * @return 16 bit integer of the inital value
 */
__int16_t ActionTracer::Packager::_float_to_int( float value ) {
	return static_cast<__int16_t>( value * 10000 );
}

/**
 * This is used to send the stored data packet in @code _packet @endcode
 * @return 0 if successful.
 */
int ActionTracer::Packager::send_packet() {
	ActionTracer::Packager::_send_packet( -1 );
	return 0;
}

/**
 * This is used to send the stored data packet in @code _packet @endcode
 * @return 0 if successful.
 */
void ActionTracer::Packager::_send_packet( int file_descriptor = -1 ) {
	if( file_descriptor == -1 ) {
		file_descriptor = _descriptor;
	}
	// Send some data
	// When this data is sent, it will be sent a single array element at a time. each element is 2 bytes (16 bits) but they are sent in reverse order i.e. TP captures 0x23ef but packager will send it as  0xef23.
#ifdef ON_PI
	if( _count == 0 ) {
		_recording_start_time = millis();
	}
	_package[0] = ( millis() - _recording_start_time ) / 1000; // Time in seconds
#endif

	_package[1] = _count;
	_count++;
	if( ( send_response = send( file_descriptor, _package, sizeof( _package ), 0 ) ) < 0 ) {
		debugPrint( "Send failed. Code %d\n Arguments were:\n\tDescriptor: %d\n\t Package: [", send_response, file_descriptor );
		for( unsigned int arprint = 0; arprint < sizeof( _package ) / sizeof( _package[0] ); arprint++ ) {
			debugPrint( " %d,", _package[arprint] );
		}
		debugPrint( "]\n\tBytes to send: %d\nError: %s\n", sizeof( _package ), strerror( errno ) );

		if( file_descriptor > 100 or file_descriptor < 0 ) {
			reset_vars();
		}

		return;
	}

#ifdef ON_PI
	if( _save ) {
		fprintf( _recording, "%8d,%7d", millis() - _recording_start_time, _count );
		for( size_t cc = PACKAGE_DATA_START; cc < PACKAGE_LENGTH; cc++ ) {
			fprintf( _recording, ",%4i", _package[cc] );
		}
		fprintf( _recording, "\n" );
	}
#endif

	debugPrint( "%8d,%7d", millis() - _recording_start_time, _count );
	for( size_t cc = PACKAGE_DATA_START; cc < PACKAGE_LENGTH; cc++ ) {
		debugPrint( ",%4i", _package[cc] );
	}
	debugPrint( "\n" );

	// for( size_t r = PACKAGE_DATA_START - 1; r < PACKAGE_LENGTH; r++ ) {
	// 	_package[r] = 0;
	// }

	_package[2] = 0;

	// return 0;
}

/**
 * This is used to add data to a package that is going to be sent. It takes an array of floats.
 * @param *data A reference to an array of floats
 * @param length number of floats in array to convert. Defaults to 4
 * @return Number of elements that have been packed.
 */
int ActionTracer::Packager::load_packet( float *data, uint8_t length = 4 ) {
	_package_pointer = ( _package[2] * 4 ) + PACKAGE_DATA_START;
	_packed			 = 0;
	for( int i = 0; i < length; i++ ) {
		_package[_package_pointer++] = _float_to_int( data[i] );
		_packed++;
	}

	_package[2] += 1; // Increase the number of device added

	return _packed;
}

/**
 * @brief Set file saving on or off. Automatically opens the file when called.
 *
 * @param value true or false
 */
void ActionTracer::Packager::save_enable( bool value ) {
	_save = value;
	this->open_file();
}

/**
 * @brief Checks if the data is being recorded on the Pi
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
	debugPrint( "Closing socket with descriptor %d\n", _descriptor );
	close( _descriptor );
}

void ActionTracer::Packager::set_descriptor( int descriptor ) {
	debugPrint( "Setting descriptor to %d\n", descriptor );
	_descriptor = descriptor;
}

void ActionTracer::Packager::dump_vars( void ) {
	printf( "\n\n\n\nSize of Packager is %d\n", sizeof( Packager ) );

	printf( "Some memory addresses: \n" );
	printf( "Port: %p\n", &_port );
	printf( "Descriptor: %p\n", &_descriptor );
	printf( "Package Pointer: %p\n", &_package_pointer );
	printf( "Package: %p\n", &_package );

	printf( "Packed: %d\n", _packed );
	printf( "Package pointer: %d\n", _package_pointer );
	printf( "Count: %d\n", _count );
	printf( "Package: [ %d", _package[0] );
	for( size_t i = 1; i < PACKAGE_LENGTH; i++ ) {
		printf( ", %d", _package[i] );
	}
	printf( "]\n" );

	printf( "Recording start time: %d\n", _recording_start_time );
	printf( "Save: %d\n", _save );

	printf( "Descriptor: %d\n", _descriptor );
	printf( "Destination: %s\n", _dest.c_str() );
	printf( "Port: %d\n", _port );
}

void ActionTracer::Packager::reset_vars( void ) {
	printf( "Error! Resetting variables\n" );
	this->dump_vars();

	for( size_t r = 0; r < PACKAGE_LENGTH; r++ ) {
		_package[r] = 0;
	}
	_package_pointer	  = PACKAGE_DATA_START;
	_port				  = 9022;
	_descriptor			  = 6;
	_count				  = 0;
	_save				  = true;
	_recording_start_time = 0;
	_packed				  = 0;
}