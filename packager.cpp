
#include "packager.h"

#include "debug_printer.h"

#include <cerrno>
#include <iomanip>
#include <unistd.h>

#ifdef ON_PI
	#include <wiringPi.h>
#endif

using namespace ActionTracer;

/**
 * @brief Construct a new Action Tracer:: Packager:: Packager object
 * @param port Destination UDP Port to send data to
 * @constructor
 */
ActionTracer::Packager::Packager( int port ) {
	_port  = port;
	_count = 0;
}

/**
 * @brief Construct a new Action Tracer:: Packager:: Packager object
 * @param port Destination UDP Port to send data to
 * @constructor
 */
ActionTracer::Packager::Packager() {
	_port  = 9022;
	_count = 0;
}

/**
 * @brief Construct a new Action Tracer:: Packager:: Packager object
 * @constructor
 */
ActionTracer::Packager::~Packager() {
	for( int i = 0; i < _client_pointer; i++ ) {
		delete _client_sockets[i];
	}
}

/**
 * @brief Setup device as TCP server
 *
 * @return int socket descriptor of the socket
 */

int ActionTracer::Packager::socket_setup() {
	_timeout.tv_sec	 = 0;
	_timeout.tv_usec = 500; // 0.5ms

	if( ( _descriptor = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
		printf( "socket failed" );
		exit( EXIT_FAILURE );
	}
	debugPrint( "TCP socket created with descriptor: %d\n", _descriptor );
	int _opt = 1;

	// This helps in manipulating options for the socket referred by the socket descriptor sockfd. This is completely optional, but it helps in reuse of address and port. Prevents error such as: “address already in use”.
	if( setsockopt( _descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_opt, sizeof( _opt ) ) ) {
		printf( "setsockopt" );
		exit( EXIT_FAILURE );
	}

	_server.sin_addr.s_addr = INADDR_ANY;
	_server.sin_family		= AF_INET;
	_server.sin_port		= htons( _port ); // My open port

	if( bind( _descriptor, ( struct sockaddr * ) &_server, sizeof( _server ) ) < 0 ) {
		printf( "bind failed" );
		exit( EXIT_FAILURE );
	}
	if( listen( _descriptor, ( MAX_CLIENTS / 2 ) ) < 0 ) {
		printf( "Error when trying to listen" );
		exit( EXIT_FAILURE );
	}

	return _descriptor;
}

/**
 * @brief Manages the TCP server by adding clients to packagers network. It accepts a client and obtains the ncessary information to create a recepient.
 *
 * @return Nothing
 */
void ActionTracer::Packager::run_socket_manager() {
	ActionClient *temp_client		= new ActionClient;
	temp_client->_socket_descriptor = accept( _descriptor, ( sockaddr * ) &temp_client->_socket_address, &temp_client->_socket_address_len ); // Blocking call waiting for new connection
	if( temp_client->_socket_descriptor < 0 ) {
		printf( "accept failed" );
		exit( EXIT_FAILURE );
	} else {
		temp_client->print_info( _client_pointer + 1 );
		for( int j = 0; j < _client_pointer; j++ ) {
			if( inet_ntoa( _client_sockets[j]->_socket_address.sin_addr ) == inet_ntoa( _client_sockets[_client_pointer]->_socket_address.sin_addr ) ) {
				delete temp_client;
				printf( "Client already connected\n" );
				return;
			}
		}
		_client_sockets[_client_pointer] = temp_client;
		_client_pointer++;
	}
}

/**
 * @brief Send data to all connected clients
 * @returns number of clients that received the data
 */
uint8_t ActionTracer::Packager::send_to_connected_devices() {
	// If the pointer is at 0, no clients are connected
	if( _client_pointer == 0 ) {
		return 0;
	}

	uint8_t recepients = 0;

	for( int i = 0; i < _client_pointer; i++, recepients++ ) {
		if( _client_sockets[i]->_socket_descriptor > 0 ) {
			send_packet( _client_sockets[i]->_socket_descriptor );
		}
	}

	return recepients;
}

/**
 * @brief Obtains the number of clients connected to the server
 * @return number of clients connected to the server
 */
uint8_t ActionTracer::Packager::_clients_connected() {
	return _client_pointer;
}

/**
 * @brief Disconnect a client from the server given its socket descriptor
 *
 * @param descriptor An open socket descriptor connected to a client
 */
void ActionTracer::Packager::disconnect_client( int8_t descriptor ) {
	for( int i = 0; i < _client_pointer; i++ ) {
		if( _client_sockets[i]->_socket_descriptor == descriptor ) {
			// Close descriptor and delete pointer in array
			close_socket( descriptor );
			delete _client_sockets[i];
			printf( "Client with address %s has been diconnected.\n", inet_ntoa( _client_sockets[i]->_socket_address.sin_addr ) );
			for( int j = i; j < _client_pointer - 1; j++ ) {
				_client_sockets[j] = _client_sockets[j + 1];
			}
			_client_pointer--;
			return;
		}
	}
}

/**
 * @brief Converts a given float value to a integer to 3 decimal places.
 *
 * @param value float to be converted to integer
 * @return 16 bit integer of the inital value
 */
__int16_t ActionTracer::Packager::_float_to_int( float value ) {
	return static_cast<__int32_t>( value * FLOAT_SCALING_FACTOR );
}

/**
 * This is used to send the stored data package to the default _descriptor
 * @return 0 if successful.
 */
int ActionTracer::Packager::send_packet() {
	ActionTracer::Packager::send_packet( _descriptor );
	return 0;
}

/**
 * This is used to send the stored data packet to a given socket described by a socket descriptor
 * @param file_descriptor An opened socket descriptor. Defults to the default descriptor
 * @return Nothing
 */
void ActionTracer::Packager::send_packet( int file_descriptor = 6 ) {
	// If no socket descriptor is given, use the last device to be added to the network
	if( file_descriptor == -1 ) {
		file_descriptor = _descriptor;
	}
	// Send some data
#ifdef ON_PI
	if( _count == 0 ) {
		_recording_start_time = millis();
	}

	// This section will get the new time but also calculate how many packets were sent
	float new_time = ( millis() - _recording_start_time ) / 1000.0;
	if( ( _package[0] - new_time ) < 1 ) {
		printf( "%d packets sent in the last second. - %.3fs\n", _count - _previous_count, new_time );
		_previous_count = _count;
	}
	_package[0] = new_time; // Time in seconds

#endif

	_package[1] = _count++;
	if( ( send_response = send( file_descriptor, _package, sizeof( _package ), 0 ) ) == -1 ) {
		if( send_response == -1 ) {
			// Client disconnected
			disconnect_client( file_descriptor );
		} else {
			perror( "Error" );
		}
		return;
	}

	_package[2] = 0;
}

/**
 * This is used to add data to a package that is going to be sent. It takes an array of floats.
 * @param data A reference to an array of floats
 * @param device_number. Device to work with. Starts from 1 and will place data in the correct position in the package.
 * @param length number of floats in array to convert. Defaults to 20
 * @return Number of elements that have been packed.
 */
int ActionTracer::Packager::load_packet( float *data, int8_t device_number = -1, uint8_t length = 19 ) {
	if( device_number <= 0 ) {
		_package_pointer = ( _package[2] * length ) + PACKAGE_DATA_START;
	} else {
		_package_pointer = ( ( device_number - 1 ) * length ) + PACKAGE_DATA_START;
	}

	_packed = 0; // Reset _packed

	for( int i = 0; i < length; i++ ) {
		_package[_package_pointer++] = data[i];
		_packed++;
	}

	_package[2] += 1; // Increase the number of device added

	return _packed;
}

/**
 * @brief Closes the a given socket given a descriptor
 * @param descriptor An open socket descriptor
 * @returns Nothing
 */
void ActionTracer::Packager::close_socket( int closing_descriptor ) {
	debugPrint( "Closing socket with descriptor %d\n", _descriptor );
	close( _descriptor );
}

/**
 * @brief Closes all the active sockets
 * @returns Nothing
 */
void ActionTracer::Packager::close_all_sockets() {
	debugPrint( "Closing all sockets\n" );

	for( int i = 0; i < _client_pointer; i++ ) {
		close_socket( _client_sockets[i]->_socket_descriptor );
	}
}

/**
 * @brief Sets the class wide descriptor to the value passed into this method
 *
 * @param descriptor opened socket descriptor
 * @returns Nothing
 */
void ActionTracer::Packager::set_descriptor( int descriptor ) {
	debugPrint( "Setting descriptor to %d\n", descriptor );
	_descriptor = descriptor;
}

/**
 * @brief Prints out all the variables in the packager including the last collected packet to be sent.
 * @returns Nothing
 */
void ActionTracer::Packager::dump_vars( void ) {
	printf( "\n\nSize of package is %d\n", sizeof( _package ) );
	printf( "Packed: %d\n", _packed );
	printf( "Package pointer: %d\n", _package_pointer );
	printf( "Count: %d\n", _count );
	printf( "Package: [ %f", _package[0] );
	for( size_t i = 1; i < PACKAGE_LENGTH; i++ ) {
		printf( ", %f", _package[i] );
	}
	printf( "]\n" );

	printf( "\nClient pointer: %d\n", _client_pointer );

	for( int i = 0; i < _client_pointer; i++ ) {
		if( _client_sockets[i]->_socket_descriptor < 0 ) {
			_client_sockets[i]->print_info( i );
		}
	}

	printf( "Descriptor: %d\n", _descriptor );
	printf( "Port: %d\n", _port );
}