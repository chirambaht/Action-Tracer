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

namespace ActionTracer {
	class Packager {
	  private:
		bool		_debug = false;
		std::string _dest;
		std::string _package;
		int			_port;
		int			_descriptor;
		__uint32_t	_count;
		std::string _float_to_string( float value, int prec );
		void		_log( std::string data );
		bool		_save = false;

	  public:
		Packager();
		Packager( std::string destination, int port );
		int send_packet();
		int load_packet( float *data, uint8_t length );
		~Packager();
		void set_debug( bool );
		void save_enable( bool );
	};

} // namespace ActionTracer