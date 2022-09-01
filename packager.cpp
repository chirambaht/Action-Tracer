#include "packager.h"

#include "debug_printer.h"

#include <cerrno>
#include <ctime>
#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/util/time_util.h>
#include <iomanip>
#include <unistd.h>

#ifdef ON_PI
	#include <wiringPi.h>
#else
	#include "wiringPi.h"
#endif

using namespace ActionTracer::Communication;

/**
 * @brief Construct a new Action Tracer:: Supervisor:: Supervisor object
 * @param port Destination UDP Port to send data to
 * @constructor
 */
ActionTracer::Communication::Supervisor::Supervisor( uint16_t port ) {
	set_server_port( port );
}

/**
 * @brief Construct a new Action Tracer:: Supervisor:: Supervisor object
 * @constructor
 */
ActionTracer::Communication::Supervisor::Supervisor() {
	set_server_port( 9022 );
}

/**
 * @brief Construct a new Action Tracer:: Supervisor:: Supervisor object
 * @constructor
 */
ActionTracer::Communication::Supervisor::~Supervisor() {
}

/**
 * @brief Setup device as TCP server
 *
 * @return int socket descriptor of the socket
 */
int ActionTracer::Communication::Supervisor::_socket_setup() {
	_server.set_descriptor( socket( AF_INET, SOCK_STREAM, 0 ) );
	if ( _server.get_descriptor() < 0 ) {
		printf( "socket failed" );
		exit( EXIT_FAILURE );
	}

	debugPrint( "TCP socket created with descriptor: %d\n", _server.get_descriptor() );

	int _opt = 1;

	// This helps in manipulating options for the socket referred by the socket descriptor sockfd. This is completely optional, but it helps in reuse of address and port. Prevents error such as:
	// “address already in use”.
	if ( setsockopt( _server.get_descriptor(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_opt, sizeof( _opt ) ) ) {
		printf( "setsockopt" );
		exit( EXIT_FAILURE );
	}

	_server.set_details( INADDR_ANY, _server.get_port() );

	if ( bind( _server.get_descriptor(), ( struct sockaddr * ) &_server._server_details, sizeof( _server.get_details() ) ) < 0 ) {
		printf( "bind failed" );
		exit( EXIT_FAILURE );
	}
	if ( listen( _server.get_descriptor(), ( MAX_CLIENTS / 2 ) ) < 0 ) {
		printf( "Error when trying to listen for connection" );
		exit( EXIT_FAILURE );
	}

	return _server.get_descriptor();
}

/**
 * @brief Manages the TCP server by adding clients to Supervisors network. It accepts a client and obtains the ncessary information to create a recepient.
 *	@throws INVALID_ARGUMENT If this Supervisor instance is still connected to a client.
 * @return Nothing
 */
uint8_t ActionTracer::Communication::Supervisor::_wait_for_connection() {
	// Check if everything else is ready
	if ( _server.get_descriptor() < 0 ) {
		printf( "Server not ready" );
		// std::__throw_invalid_argument( "Server not ready for use, please run _socket_setup() first" );
		return -1;
	}

	// Show connection IP
	debugPrint( "Waiting for connection..." );
	ActionServerClient *temp_client = new ActionServerClient();
	temp_client->set_descriptor( accept( _server.get_descriptor(), ( sockaddr * ) &temp_client->address, &temp_client->_address_len ) ); // Blocking call waiting for new connection

	if ( temp_client->get_descriptor() < 0 ) {
		printf( "accept failed" );
		exit( EXIT_FAILURE );
	} else {
		_server.connect_client( temp_client );
	}
	set_ready( true );
	return temp_client->get_descriptor();
}

/**
 * @brief Inits a Supervisor instance and only continues if a client is connected to the server.
 */
void ActionTracer::Communication::Supervisor::initialize() {
	_socket_setup();
	// _wait_for_connection();
}

/**
 * @brief Inits a Supervisor instance and only continues if a client is connected to the server.
 */
void ActionTracer::Communication::Supervisor::initialize( bool run_server ) {
	if ( run_server ) {
		_wait_for_connection();
	} else {
		_socket_setup();
	}
}

/**
 * @brief Disconnect a client from the server given its socket descriptor
 * @throws INVALID_ARGUMENT if the client is not connected to the server
 */
void ActionTracer::Communication::Supervisor::disconnect() {
	if ( _server.get_descriptor() < 0 ) {
		printf( "Server not ready" );
		// std::__throw_invalid_argument( "Server not ready for use, I can not disconnect when I haven't connected!" );
		return;
	}

	_server.disconnect_all_clients();

	// close_socket( _server.get_descriptor() );
	set_ready( false );
}

/**
 * This is used to send the stored data packet to all clients connected. This will first load the data packet into a buffer and then send it to all clients.
 * @return Nothing
 * @throws INVALID_ARGUMENT If there is no device connected to the system's network.
 */
int ActionTracer::Communication::Supervisor::send_packet( ActionDataPackage *device_packet ) {
	int ret = load_packet( device_packet );
	send_packet();
	return ret;
}

/**
 * This is used to send the stored data packet to a given socket described by a socket descriptor
 * @return Nothing
 * @throws INVALID_ARGUMENT If there is no device connected to the system's network.
 */
void ActionTracer::Communication::Supervisor::send_packet() {
	// If no socket descriptor is given, use the last device to be added to the network
	if ( !get_ready() ) {
		// throw std::invalid_argument( "No device is connected to the system's network." );
		return;
	}

	_net_package.set_packet_number( ++_count );

	auto		   timestamp = new google::protobuf::Timestamp{};
	struct timeval tv;
	gettimeofday( &tv, NULL );

	// timestamp->set_seconds( time(NULL) );
	timestamp->set_seconds( tv.tv_sec );
	timestamp->set_nanos( tv.tv_usec * 1000 );
	_net_package.set_allocated_send_time( timestamp );

	if ( !_net_package.IsInitialized() ) {
		// throw std::invalid_argument( "Packet is not ready to be sent" );
		return;
	}

	// Packet size
	// printf( "Packet size to be sent: %ld\n", sizeof( _net_package.SerializeAsString() ) );
	_server.send_packet( &_net_package );

	// Reset the packet
	_net_package.Clear();
}

/**
 * This is used to add data to a package that is going to be sent. It takes an array of floats.
 * @param data A reference to an array of floats
 * @param device_number. Device to work with. Starts from 1 and will place data in the correct position in the package.
 * @param length number of floats in array to convert. Defaults to 20
 * @return Number of elements that have been packed.
 */
int ActionTracer::Communication::Supervisor::load_packet( ActionDataPackage *device_packet ) {
	_packed = 0;

	// ActionDataNetworkPackage_ActionDeviceData  *device_data		= new ActionDataNetworkPackage_ActionDeviceData();

	ActionDataNetworkPackage_ActionDeviceData *device_data = _net_package.add_device_data();
	device_data->set_device_identifier_contents( device_packet->device_identifier_contents );

	_packed++;

	for ( int i = 0; i < DATA_ELEMENTS; i++ ) {
		device_data->add_data( device_packet->data[i] );
		_packed++;
	}

	return _packed;
}

/**
 * @brief Closes the a given socket given a descriptor
 * @param descriptor An open socket descriptor
 * @returns Nothing
 */
void ActionTracer::Communication::Supervisor::close_socket( uint8_t closing_descriptor ) {
	debugPrint( "Closing socket with descriptor %d\n", _server.get_descriptor() );
	close( _server.get_descriptor() );
}

/**
 * @brief Set the server's port number
 * @param port The port number to set the server to
 * @returns Nothing
 */
void ActionTracer::Communication::Supervisor::set_server_port( const uint16_t port ) {
	_server.set_port( port );
}

/**
 * @brief Set the server's port number
 * @param port The port number to set the server to
 * @returns Nothing
 */
uint16_t ActionTracer::Communication::Supervisor::get_server_port() const {
	return _server.get_port();
}

/**
 * @brief Prints out all the variables in the Supervisor including the last collected packet to be sent.
 * @returns Nothing
 */
void ActionTracer::Communication::Supervisor::dump_vars( void ) {
	printf( "\n\nSize of package is %db\n", sizeof( _net_package.ByteSizeLong() ) );
	printf( "Packed: %d\n", _packed );
	printf( "Count: %d\n", _count );

	printf( "%s", _net_package.ShortDebugString().c_str() );

	printf( "Descriptor: %d\n", _client.get_descriptor() );
	printf( "Port: %d\n", _server.get_port() );

	_server.dump_vars();
}

/**
 * @brief Sets the device into a given ready mode
 * @param status The status to set the device to
 * @returns Nothing
 */
void ActionTracer::Communication::Supervisor::set_ready( bool status ) {
	_ready = status;
}

/**
 * @brief Obtains the ready status of the device
 * @returns _ready The ready status of the device
 */
bool ActionTracer::Communication::Supervisor::get_ready() const {
	return _ready;
}

/**
 * @brief Construct a new Action Tracer:: Communication:: Action Server:: Action Server object
 *
 */
ActionTracer::Communication::ActionServer::ActionServer() {
}

/**
 * @brief Construct a new Action Tracer:: Communication:: Action Server:: Action Server object
 * @param my_address address to bind to
 * @param port The port to listen on
 */
ActionTracer::Communication::ActionServer::ActionServer( sockaddr_in my_address, uint16_t port ) : _port( port ), address( my_address ) {
}

/**
 * @brief Destroy a Action Tracer:: Communication:: Action Server:: Action Server object
 */
ActionTracer::Communication::ActionServer::~ActionServer() {
}

/**
 * @brief Gets the number of bytes in the address
 * @returns socklen_t address size
 */
socklen_t ActionTracer::Communication::ActionServer::get_socket_address_length() const {
	return _address_len;
}

/**
 * @brief Gets the details of the device to send data from. The port will be obtained separately.
 * @returns sockaddr_in The details of the device to send data from
 */
size_t ActionTracer::Communication::ActionServer::get_clients_connected() const {
	return _clients.size();
}

/**
 * @brief Gets the details of the device to send data from. The port will be obtained separately.
 * @returns sockaddr_in The details of the device to send data from
 */
struct sockaddr_in ActionTracer::Communication::ActionServer::get_details() const {
	return _server_details;
}

/**
 * @brief Sets the details of the device to send data from
 * @param address IPV4 address to send data from on an Action Device
 * @param port Local machine port to send data to
 * @returns Nothing
 */
void ActionTracer::Communication::ActionServer::set_details( in_addr_t address, uint16_t port ) {
	_server_details.sin_addr.s_addr = address;
	_server_details.sin_family		= AF_INET;
	_server_details.sin_port		= htons( port );
}

/**
 * @brief Sets the network address into of the device
 * @param status The status to set the device to
 * @returns Nothing
 */
void ActionTracer::Communication::ActionServer::set_address( const sockaddr_in my_address ) {
	address = my_address;
}

/**
 * @brief Obtains the network address of the device
 * @returns _ready The ready status of the device
 */
sockaddr_in ActionTracer::Communication::ActionServer::get_address() const {
	return address;
}

/**
 * @brief Gets the port of the device
 * @returns uint16_t The port of the device
 */
uint16_t ActionTracer::Communication::ActionServer::get_port() const {
	return _port;
}

/**
 * @brief Sets the port of the device
 * @param port The status to set the device to
 * @returns Nothing
 */
void ActionTracer::Communication::ActionServer::set_port( const uint16_t port ) {
	_port = port;
}

/**
 * @brief Gets the descriptor of the server
 * @returns int The descriptor of the server
 */
uint8_t ActionTracer::Communication::ActionServer::get_descriptor() const {
	return _descriptor;
}

/**
 * @brief Sets the descriptor of the server
 * @param descriptor The descriptor of the server
 * @returns Nothing
 */
void ActionTracer::Communication::ActionServer::set_descriptor( const int descriptor ) {
	_descriptor = descriptor;
}

/**
 * @brief Adds a client to the list of clients connected to the server
 * @param ActionServerClient The client to add to the list
 * @returns size_t Number of clients connected to the server
 */
uint8_t ActionTracer::Communication::ActionServer::connect_client( ActionServerClient *client ) {
	_clients.push_back( *client );
	return _clients.size();
}

/**
 * @brief Disconnect a single client from the server
 * @param ActionServerClient client to disconnect
 */
void ActionTracer::Communication::ActionServer::disconnect_client( ActionServerClient *client ) {
	client->send_disconnect_notification();
	close( client->get_descriptor() );

	// need to remove from vector
}

/**
 * @brief Disconnect all clients from the server
 */
void ActionTracer::Communication::ActionServer::disconnect_all_clients() {
	for ( auto client : _clients ) {
		client.disconnect();
	}
	_clients.clear();
}

/**
 * @brief Prints out all the variables in the server.
 * @returns Nothing
 */
void ActionTracer::Communication::ActionServer::dump_vars() {
	printf( "\nAction Server - %s:%d\n", inet_ntoa( address.sin_addr ), ntohs( address.sin_port ) );
	printf( "Descriptor: %d\n", _descriptor );

	printf( "Clients: %d\n", _clients.size() );

	for ( auto client : _clients ) {
		client.dump_vars();
	}
}

/**
 * @brief Send a packet to all clients connected to the server
 * @param package A pointer to the data packet to send
 */
int16_t ActionTracer::Communication::ActionServer::send_packet( ActionDataNetworkPackage *package ) {
	if ( _clients.size() == 0 ) {
		return 0;
	}

	for ( auto client : _clients ) {
		int res = client.send_packet( package );
		if ( res == -1 ) {
			disconnect_all_clients();
			break;
		}
	}

	return package->ByteSizeLong();
}

/**
 * @brief Send a packet to a client connected to the server
 * @param package A pointer to the data packet to send
 */
int16_t ActionTracer::Communication::ActionServer::send_packet( ActionDataNetworkPackage *package, ActionServerClient *client ) {
	return client->send_packet( package );
}

/**
 * @brief Construct a new Action Tracer:: Communication:: Action Server:: Action Server Client object
 *
 */
ActionTracer::Communication::ActionServerClient::ActionServerClient() {
}

/**
 * @brief Construct a new Action Tracer:: Communication:: Action Server:: Action Server Client object
 * @param client_address address to bind to
 * @param descriptor The descriptor to keep track of the client
 */
ActionTracer::Communication::ActionServerClient::ActionServerClient( sockaddr_in client_address, uint8_t descriptor ) : _descriptor( descriptor ), address( client_address ) {
}

/**
 * @brief Destroy a Action Tracer:: Communication:: Action Server Client:: Action Server Client object
 */
ActionTracer::Communication::ActionServerClient::~ActionServerClient() {
}

/**
 * @brief Gets the socket addess length of the client
 * @returns int The socket addess length of the client
 */
socklen_t ActionTracer::Communication::ActionServerClient::get_socket_address_length() const {
	return _address_len;
}

/**
 * @brief Gets the descriptor of the client
 * @returns int The descriptor of the client
 */
uint8_t ActionTracer::Communication::ActionServerClient::get_descriptor() const {
	return _descriptor;
}

/**
 * @brief Sets the descriptor of the client
 * @param descriptor The descriptor of the client
 * @returns Nothing
 */
void ActionTracer::Communication::ActionServerClient::set_descriptor( const int descriptor ) {
	_descriptor = descriptor;
}

/**
 * @brief Send a packet to the client via socket
 * @returns packet pointer to the packet to send
 */
int16_t ActionTracer::Communication::ActionServerClient::send_packet( ActionDataNetworkPackage *packet ) {
	if ( !packet->IsInitialized() ) {
		// throw std::invalid_argument( "Packet is not ready to be sent" );
		return;
	}

	if ( ( send_response = send( _descriptor, packet->SerializeAsString().c_str(), packet->ByteSizeLong(), 0 ) ) == -1 ) {
		if ( send_response == -1 ) {
			// Client disconnected
			client_disconnected();
			return -1;
		} else {
			perror( "Error" );
		}
	}
	return send_response;
}

/**
 * @brief Disconnects the client from the server without notification
 * @returns Nothing
 */
void ActionTracer::Communication::ActionServerClient::client_disconnected() {
	close( get_descriptor() );
}

/**
 * @brief Disconnects the client from the server with notification
 * @returns Nothing
 */
void ActionTracer::Communication::ActionServerClient::disconnect() {
	send_disconnect_notification();
	close( get_descriptor() );
}

/**
 * @brief Sends the disconnect notification to the client. This notification is a standard message with no and packet number set to 0 and as no data.
 * @returns Nothing
 */
void ActionTracer::Communication::ActionServerClient::send_disconnect_notification() {
	ActionDataNetworkPackage disconnect_packet = ActionDataNetworkPackage();
	disconnect_packet.set_packet_number( 0 );
	send_packet( &disconnect_packet );
}

/**
 * @brief Prints out all the variables in a client.
 * @returns Nothing
 */
void ActionTracer::Communication::ActionServerClient::dump_vars() {
	printf( "\nAction Server Client - %s:%d\n", inet_ntoa( address.sin_addr ), ntohs( address.sin_port ) );
	printf( "Descriptor: %d\n", _descriptor );
}