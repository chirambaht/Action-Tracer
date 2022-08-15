#include "act_definitions.h"

#include <arpa/inet.h> //inet_addr
#include <cstdio>

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
		ActionDataNetworkPackage _net_package;
		sockaddr_in				 _server_address;
		unsigned int			 _server_address_len = sizeof( sockaddr_in );
		int						 _server_descriptor	 = 0;

		__uint8_t _packed		   = 0;
		__uint8_t _package_pointer = PACKAGE_DATA_START;

		ActionClient *_client;
		__uint32_t	  _port	 = DEFAULT_PORT;
		__uint16_t	  _count = 0;

		__int8_t send_response;

		bool			   _ready = false;
		struct sockaddr_in _server;

		uint8_t _wait_for_connection();
		int		_socket_setup( void );

	  public:
		Packager();
		Packager( int );
		~Packager();

		void send_packet( void );
		void initialize();
		void disconnect();

		void close_socket( int );
		void dump_vars( void );

		int load_packet( ActionDataPackage * );

		void	set_server_descriptor( int );
		uint8_t get_server_descriptor() const;

		void	set_client_descriptor( int );
		uint8_t get_client_descriptor() const;

		void set_ready( bool );
		bool get_ready() const;
	};

} // namespace ActionTracer