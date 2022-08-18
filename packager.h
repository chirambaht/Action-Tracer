#ifndef PACKAGER_H
#define PACKAGER_H
#include "act_definitions.h"

#include <arpa/inet.h> //inet_addr
#include <cstdint>
#include <cstdio>
#include <vector>

namespace ActionTracer::Communication {

	class ActionServer {
	  private:
		socklen_t		   _address_len = sizeof( sockaddr_in );
		struct sockaddr_in _server_details;

		uint8_t	 _descriptor = 0;
		uint16_t _port		 = 0;

		std::vector<ActionServerClient> _clients;

	  public:
		sockaddr_in address;

		ActionServer();
		ActionServer( sockaddr_in, uint16_t );
		~ActionServer();

		uint8_t send_packet( const ActionDataNetworkPackage &package );
		uint8_t send_packet( const ActionDataNetworkPackage &package, const ActionServerClient &client );

		uint8_t connect_client( ActionServerClient *client );
		void	disconnect_client( ActionServerClient &client );
		void	disconnect_all_clients();

		uint16_t get_address_length() const;

		socklen_t get_socket_address_length() const;

		uint8_t get_clients_connected() const;

		uint16_t get_port() const;
		void	 set_port( const uint16_t port );

		uint8_t get_descriptor() const;
		void	set_descriptor( const int descriptor );

		struct sockaddr_in get_details() const;
		void			   set_details( in_addr_t, uint16_t );

		sockaddr_in get_address() const;
		void		set_address( const sockaddr_in );

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

		uint8_t _packed = 0;

		ActionServerClient _client;

		uint16_t _count = 0;

		int8_t send_response;

		bool _ready = false;

		uint8_t _wait_for_connection();
		int		_socket_setup( void );

	  public:
		Supervisor();
		Supervisor( uint16_t );
		~Supervisor();

		void send_packet( void );
		int	 send_packet( ActionDataPackage	*);
		void initialize();

		void close_socket( uint8_t );
		void dump_vars( void );

		int load_packet( ActionDataPackage * );

		void disconnect();

		void set_ready( bool );
		bool get_ready() const;

		void	 set_server_port( uint16_t );
		uint16_t get_server_port() const;
	};

} // namespace ActionTracer::Communication
#endif