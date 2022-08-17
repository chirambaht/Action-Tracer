#include "act_definitions.h"

#include <arpa/inet.h> //inet_addr
#include <cstdio>
#include <vector>

namespace ActionTracer::Communication {

	class ActionServer {
	  private:
		socklen_t _address_len = sizeof( sockaddr_in );
		int		  _descriptor  = 0;
		uint16_t  _port		   = 0;

		std::vector<ActionServerClient> _clients;

		void set_descriptor( const uint8_t descriptor );
		void set_port( uint8_t port );

	  public:
		sockaddr_in address;

		ActionServer();
		ActionServer( sockaddr_in, uint16_t );
		~ActionServer();

		uint8_t send_packet( const ActionDataNetworkPackage &package );
		uint8_t send_packet( const ActionDataNetworkPackage &package, const ActionServerClient &client );

		uint8_t	 get_descriptor() const;
		uint16_t get_address_length() const;

		void	  disconnect_client( ActionServerClient &client );
		socklen_t get_socket_address_length() const;
		uint8_t	  get_clients_connected() const;
		uint8_t	  connect_client( ActionServerClient *client );

		void dump_vars();
	};

	class ActionServerClient {
	  private:
		sockaddr_in _address;

		uint8_t _descriptor = 0;

	  public:
		socklen_t	_address_len = sizeof( sockaddr_in );
		sockaddr_in address;
		ActionServerClient();

		ActionServerClient( sockaddr_in, uint8_t );

		// ActionServerClient( ActionServerClient const & );			 // Copy constructor
		// ActionServerClient &operator=( ActionServerClient const & ); // Copy assignment operator
		// ActionServerClient( ActionServerClient && );				 // Move constructor
		// ActionServerClient &operator=( ActionServerClient && );		 // Move assignment operator

		~ActionServerClient();

		void dump_vars();

		uint8_t	  get_descriptor() const;
		socklen_t get_socket_address_length() const;

		void set_descriptor( const int descriptor );

		uint16_t send_packet( const ActionDataNetworkPackage &packet );

		void initialize();

		void disconnect();
	};

	class Supervisor {
	  private:
		ActionServer _server;

		ActionDataNetworkPackage _net_package;

		__uint8_t _packed = 0;

		ActionServerClient _client;

		__uint16_t _count = 0;

		__int8_t send_response;

		bool _ready = false;

		uint8_t _wait_for_connection();
		int		_socket_setup( void );

	  public:
		Supervisor();
		~Supervisor();

		void send_packet( void );
		void initialize();

		void close_socket( int );
		void dump_vars( void );

		int load_packet( ActionDataPackage * );

		void set_ready( bool );
		bool get_ready() const;
	};

} // namespace ActionTracer::Communication