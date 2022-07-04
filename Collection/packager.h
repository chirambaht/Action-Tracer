#include <arpa/inet.h> //inet_addr
#include <cstdio>

#ifndef DEVICES_IN_USE
	#define DEVICES_IN_USE	   3 // 3 IMUs
	#define DATA_ELEMENTS	   DEVICES_IN_USE * 4
	#define PACKAGE_LENGTH	   DATA_ELEMENTS + 3
	#define PACKAGE_DATA_START 3 // 3 is the number of elements in the header

/*
	|  HEADER  |           DATA           |
	|  0 -> 2  |  3 -> DATA_ELEMENTS - 1  |

	HEADER 0: Time
	HEADER 1: Count
	HEADER 2: Devices Connected
*/
#endif
#define MAX_CLIENTS 10

namespace ActionTracer {

	typedef struct ActionClient {
		sockaddr_in	 _socket_address;
		unsigned int _socket_address_len = sizeof( sockaddr_in );
		int			 _socket_descriptor	 = 0;

		void print_info( int index ) {
			printf( "%d. Address: %s:%d, Descriptor: %d\n", index, inet_ntoa( _socket_address.sin_addr ), ntohs( _socket_address.sin_port ), _socket_descriptor );
		}
	} ActionClient;

	class Packager {
	  private:
		__int16_t _package[PACKAGE_LENGTH] = { 0 };
		__uint8_t _packed				   = 0;
		__uint8_t _client_pointer		   = 0;
		__uint8_t _package_pointer		   = PACKAGE_DATA_START;

		ActionClient		 *_client_sockets[MAX_CLIENTS];
		int				   _port;
		__uint8_t		   _descriptor;
		__uint16_t		   _count;
		__uint16_t		   _recording_start_time;
		struct sockaddr_in _server;
		struct timeval	   _timeout; // a 5ms timeout

		void	  _send_packet( int );
		__int16_t _float_to_int( float value );

	  public:
		Packager();
		~Packager();
		Packager( int port );

		__int8_t send_response;

		int send_packet();

		int	 socket_setup();
		void run_socket_manager();
		void send_to_connected_devices();

		void	close_socket( void );
		void	set_descriptor( int );
		void	dump_vars( void );
		int		load_packet( float *data, uint8_t length );
		uint8_t _clients_connected();
	};

} // namespace ActionTracer