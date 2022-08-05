#include <arpa/inet.h> //inet_addr
#include <cstdio>

#ifndef DEVICES_IN_USE
	#define DEVICES_IN_USE	   3				   // 3 IMUs
	#define DATA_ELEMENTS	   DEVICES_IN_USE * 20 // 20 data elements per IMU
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
		sockaddr_in	 _action_client_address;
		unsigned int _action_client_address_len = sizeof( sockaddr_in );
		int			 _action_client_descriptor	= 0;

		/**
		 * @brief Print out the socket Address, Port and Descriptor
		 *
		 * @param index The index of the device.
		 */
		void print_info( int index = -1 ) {
			printf( "%d. Address: %s:%d, Descriptor: %d\n", index, inet_ntoa( _action_client_address.sin_addr ), ntohs( _action_client_address.sin_port ), _action_client_descriptor );
		}
	} ActionClient;

	class Packager {
	  private:
		sockaddr_in	 _server_address;
		unsigned int _server_address_len = sizeof( sockaddr_in );
		int			 _server_descriptor	 = 0;

		float	  _package[PACKAGE_LENGTH] = { 0 };
		uint16_t  _package_size			   = PACKAGE_LENGTH * sizeof( float );
		char	  buf[128];
		__uint8_t _packed		   = 0;
		__uint8_t _package_pointer = PACKAGE_DATA_START;

		ActionClient *_client;
		__uint32_t	  _port			  = DEFAULT_PORT;
		__uint16_t	  _count		  = 0;
		__uint16_t	  _previous_count = 0;
		__uint16_t	  _recording_start_time;

		float			   _clocked = 0.0;
		bool			   _ready	= false;
		struct sockaddr_in _server;
		struct timeval	   _timeout; // a 5ms timeout

		__int16_t _float_to_int( float );

	  public:
		Packager();
		Packager( int );
		~Packager();

		__int8_t send_response;

		void	send_packet( void );
		uint8_t wait_for_connection();
		int		socket_setup( void );
		void	disconnect();

		void close_socket( int );
		void close_all_sockets();
		void dump_vars( void );
		int	 load_packet( float *, int8_t, uint8_t );

		void	set_server_descriptor( int );
		uint8_t get_server_descriptor() const;

		void	set_client_descriptor( int );
		uint8_t get_client_descriptor() const;

		void set_ready( bool );
		bool get_ready() const;
	};

} // namespace ActionTracer