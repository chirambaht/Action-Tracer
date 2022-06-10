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

namespace ActionTracer {
	class Packager {
	  private:
		std::string _dest;

		__int16_t		   _package[PACKAGE_LENGTH] = { 0 }; // For a start this will be a 4 (data points) * 4 (devices) integer
		size_t			   _package_pointer			= PACKAGE_DATA_START;
		int				   _port;
		int				   _descriptor = 6;
		__uint32_t		   _count;
		bool			   _save = false;
		FILE			  *_recording;
		int32_t			   _recording_start_time = 0;
		struct sockaddr_in _server;
		int				   _packed = 0;
		__int16_t		   _float_to_int( float value );

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
		void set_descriptor( int );
		void dump_vars( void );
		void reset_vars( void );
	};

} // namespace ActionTracer