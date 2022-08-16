
#include "packager.h"

#include "debug_printer.h"

#include <cerrno>
#include <ctime>
#include <google/protobuf/timestamp.pb.h>
#include <iomanip>
#include <unistd.h>

#ifdef ON_PI
	#include <wiringPi.h>
#else
	#include "wiringPi.h"
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
	if( _client != nullptr )
		delete _client;
}

/**
 * @brief Setup device as TCP server
 *
 * @return int socket descriptor of the socket
 */
int ActionTracer::Packager::_socket_setup() {
	if( ( _server_descriptor = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
		printf( "socket failed" );
		exit( EXIT_FAILURE );
	}
	debugPrint( "TCP socket created with descriptor: %d\n", _server_descriptor );
	int _opt = 1;

	// This helps in manipulating options for the socket referred by the socket descriptor sockfd. This is completely optional, but it helps in reuse of address and port. Prevents error such as: “address already in use”.
	if( setsockopt( _server_descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_opt, sizeof( _opt ) ) ) {
		printf( "setsockopt" );
		exit( EXIT_FAILURE );
	}

	_server.sin_addr.s_addr = INADDR_ANY;
	_server.sin_family		= AF_INET;
	_server.sin_port		= htons( _port ); // My open port

	if( bind( _server_descriptor, ( struct sockaddr * ) &_server, sizeof( _server ) ) < 0 ) {
		printf( "bind failed" );
		exit( EXIT_FAILURE );
	}
	if( listen( _server_descriptor, ( MAX_CLIENTS / 2 ) ) < 0 ) {
		printf( "Error when trying to listen for connection" );
		exit( EXIT_FAILURE );
	}

	return _server_descriptor;
}

/**
 * @brief Manages the TCP server by adding clients to packagers network. It accepts a client and obtains the ncessary information to create a recepient.
 *	@throws INVALID_ARGUMENT If this Packager instance is still connected to a client.
 * @return Nothing
 */
uint8_t ActionTracer::Packager::_wait_for_connection() {
	if( _client != nullptr ) {
		throw std::invalid_argument( "A is already connected to this Packager." );
		return _client->_action_client_descriptor;
	}

	ActionClient *temp_client			   = new ActionClient;
	temp_client->_action_client_descriptor = accept( _server_descriptor, ( sockaddr * ) &temp_client->_action_client_address, &temp_client->_action_client_address_len ); // Blocking call waiting for new connection

	if( temp_client->_action_client_descriptor < 0 ) {
		printf( "accept failed" );
		exit( EXIT_FAILURE );
	} else {
		_client = temp_client;
	}
	set_ready( true );
	return temp_client->_action_client_descriptor;
}

/**
 * @brief Inits a packager instance and only continues if a client is connected to the server.
 */
void ActionTracer::Packager::initialize() {
	_socket_setup();
	_wait_for_connection();
}

/**
 * @brief Disconnect a client from the server given its socket descriptor
 * @throws INVALID_ARGUMENT if the client is not connected to the server
 */
void ActionTracer::Packager::disconnect() {
	if( _client == nullptr ) {
		throw std::invalid_argument( "No device is connected to the system's network." );
		return;
	}

	close_socket( _client->_action_client_descriptor );
	delete _client;
	printf( "Client with address %s has been diconnected.\n", inet_ntoa( _client->_action_client_address.sin_addr ) );
	set_ready( false );
	return;
}

/**
 * This is used to send the stored data packet to a given socket described by a socket descriptor
 * @return Nothing
 * @throws INVALID_ARGUMENT If there is no device connected to the system's network.
 */
void ActionTracer::Packager::send_packet() {
	// If no socket descriptor is given, use the last device to be added to the network
	if( _client == nullptr ) {
		throw std::invalid_argument( "No device is connected to the system's network." );
	}

	_net_package.set_packet_number( _count++ );

	// Set time
	google::protobuf::Timestamp t;
	t.set_seconds( std::time( nullptr ) );

	_net_package.set_allocated_send_time( &t );

	if( ( send_response = send( _client->_action_client_descriptor, _net_package.SerializeAsString().c_str(), _net_package.ByteSizeLong(), 0 ) ) == -1 ) {
		if( send_response == -1 ) {
			// Client disconnected
			disconnect();
		} else {
			perror( "Error" );
		}
		return;
	}
}

/**
 * This is used to add data to a package that is going to be sent. It takes an array of floats.
 * @param data A reference to an array of floats
 * @param device_number. Device to work with. Starts from 1 and will place data in the correct position in the package.
 * @param length number of floats in array to convert. Defaults to 20
 * @return Number of elements that have been packed.
 */
int ActionTracer::Packager::load_packet( ActionDataPackage *device_packet ) {
	_packed = 0;
	_net_package.set_device_identifier_contents( device_packet->device_identifier_contents );
	_packed++;
	for( int i = 0; i < DATA_ELEMENTS; i++ ) {
		_net_package.add_data( device_packet->data[i] );
		_packed++;
	}
	return _packed;
}

/**
 * @brief Closes the a given socket given a descriptor
 * @param descriptor An open socket descriptor
 * @returns Nothing
 */
void ActionTracer::Packager::close_socket( int closing_descriptor ) {
	debugPrint( "Closing socket with descriptor %d\n", _server_descriptor );
	close( _server_descriptor );
}

/**
 * @brief Prints out all the variables in the packager including the last collected packet to be sent.
 * @returns Nothing
 */
void ActionTracer::Packager::dump_vars( void ) {
	printf( "\n\nSize of package is %d\n", sizeof( _net_package.ByteSizeLong() ) );
	printf( "Packed: %d\n", _packed );
	printf( "Package pointer: %d\n", _package_pointer );
	printf( "Count: %d\n", _count );

	printf( "%s", _net_package.DebugString().c_str() );

	printf( "Descriptor: %d\n", _server_descriptor );
	printf( "Port: %d\n", _port );

	if( _client != nullptr ) {
		_client->print_info();
	} else {
		printf( "No client connected at the moment\n" );
	}
}

/**
 * @brief Sets the server descriptor to a given value
 * @param server_descriptor An open socket descriptor for the server
 * @returns Nothing
 */
void ActionTracer::Packager::set_server_descriptor( int server_descriptor ) {
	_server_descriptor = server_descriptor;
}

/**
 * @brief Obtains the server descriptor
 * @returns _server_descriptor The server descriptor
 */
uint8_t ActionTracer::Packager::get_server_descriptor() const {
	return _server_descriptor;
}

/**
 * @brief Sets the client descriptor to a given value
 * @param client_descriptor An open socket descriptor for the client
 * @returns Nothing
 * @throws INVALID_ARGUMENT Thrown if no client is connected to the system's network.
 */
void ActionTracer::Packager::set_client_descriptor( int client_descriptor ) {
	if( _client == nullptr ) {
		throw std::invalid_argument( "No device is connected to the system's network." );
		return;
	} else {
		_client->_action_client_descriptor = client_descriptor;
	}
}

/**
 * @brief Obtains the client descriptor
 * @returns _client_descriptor The client descriptor if it is connected to the system's network.
 * @throws INVALID_ARGUMENT Thrown if no client is connected to the system's network.
 */
uint8_t ActionTracer::Packager::get_client_descriptor() const {
	if( _client == nullptr ) {
		throw std::invalid_argument( "No device is connected to the system's network." );
		return 0;
	} else {
		return _client->_action_client_descriptor;
	}
}

/**
 * @brief Sets the device into a given ready mode
 * @param status The status to set the device to
 * @returns Nothing
 */
void ActionTracer::Packager::set_ready( bool status ) {
	_ready = status;
}

/**
 * @brief Obtains the ready status of the device
 * @returns _ready The ready status of the device
 */
bool ActionTracer::Packager::get_ready() const {
	return _ready;
}