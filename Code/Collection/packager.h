#include <arpa/inet.h> //inet_addr
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include <thread>

#define SEND_INT

namespace ActionTracer {
	class Packager {
	  private:
		std::string _dest;

#ifdef SEND_INT
		const size_t _package_packets = 12;
		__int16_t	 _package[12]	  = { 0 }; // For a start this will be a 4 (data points) * 3 (devices) integer
		size_t		 _package_pointer = 0;
#else
		std::string _package;
#endif
		int				   _port;
		int				   _descriptor;
		__uint32_t		   _count;
		std::string		   _float_to_string( float value, int prec );
		__int16_t		   _float_to_int( float value );
		void			   _log( std::string data );
		bool			   _save = false;
		FILE				 *_recording;
		int32_t			   _recording_start_time = 0;
		struct sockaddr_in _server;
		std::thread		   sender;

	  public:
		size_t _number_of_devices = 3;
		Packager();
		Packager( std::string destination, int port );
		void init_udp();
		void init_tcp();
		int	 send_packet();
		int	 load_packet( float *data, uint8_t length );
		~Packager();
		void save_enable( bool );
		bool save_status();
		void close_file( void );
		void _send_packet();
		void open_file( void );
		void close_socket( void );
	};

} // namespace ActionTracer