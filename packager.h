#include <arpa/inet.h> //inet_addr
#include <cstdio>

#ifndef DEVICES_IN_USE
	#define DEVICES_IN_USE	   3 // 3 IMUs
	#define DATA_ELEMENTS	   DEVICES_IN_USE * 19
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
#define DEFAULT_PORT		 9022
#define MAX_CLIENTS			 10
#define FLOAT_SCALING_FACTOR 10000

namespace ActionTracer {

	typedef struct ActionClient {
		sockaddr_in	 _socket_address;
		unsigned int _socket_address_len = sizeof( sockaddr_in );
		int			 _socket_descriptor	 = 0;

		/**
		 * @brief Print out the socket Address, Port and Descriptor
		 *
		 * @param index The index of the device.
		 */
		void print_info( int index = -1 ) {
			printf( "%d. Address: %s:%d, Descriptor: %d\n", index, inet_ntoa( _socket_address.sin_addr ), ntohs( _socket_address.sin_port ), _socket_descriptor );
		}
	} ActionClient;

	class Packager {
	  private:
		float	  _package[PACKAGE_LENGTH] = { 0 };
		char	  buf[128];
		__uint8_t _packed		   = 0;
		__uint8_t _client_pointer  = 0;
		__uint8_t _package_pointer = PACKAGE_DATA_START;

		ActionClient		 *_client_sockets[MAX_CLIENTS] = { 0 };
		uint32_t		   _port						= DEFAULT_PORT;
		__uint8_t		   _descriptor					= 0;
		__uint16_t		   _count						= 0;
		__uint16_t		   _previous_count				= 0;
		__uint16_t		   _recording_start_time;
		struct sockaddr_in _server;
		struct timeval	   _timeout; // a 5ms timeout

		__int16_t _float_to_int( float );

	  public:
		Packager();
		Packager( int );
		~Packager();

		__int8_t send_response;

		int	 send_packet( void );
		void send_packet( int );

		int		socket_setup( void );
		void	disconnect_client( int8_t );
		void	run_socket_manager( void );
		uint8_t send_to_connected_devices( void );

		void	close_socket( int );
		void	close_all_sockets();
		void	set_descriptor( int );
		void	dump_vars( void );
		int		load_packet( float *, int8_t, uint8_t );
		uint8_t _clients_connected( void );
	};

} // namespace ActionTracer