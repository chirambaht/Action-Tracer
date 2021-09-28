#include <arpa/inet.h> //inet_addr
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <string.h> //strlen
#include <sys/socket.h>

namespace IMU_Tools {
class Packager {
private:
  std::string _dest;
  int _port;
  int _descriptor;
  __uint32_t _count;

  std::string _float_to_string(float value);

public:
  Packager();
  Packager(std::string destination, int port);
  int send_packet(float *data);
  ~Packager();
};

} // namespace IMU_Tools