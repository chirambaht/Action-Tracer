#include <arpa/inet.h> //inet_addr
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
		std::string	  _dest;
		int			  _port;
		int			  _descriptor;
		__uint32_t	  _count;
		std::ofstream logger;
		std::string	  _float_to_string( float value, int prec );
		void		  _log( std::string data );

	  public:
		Packager();
		Packager( std::string destination, int port );
		int send_packet( float *data, uint8_t length );
		~Packager();
	};

} // namespace ActionTracer