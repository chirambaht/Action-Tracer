#include "action_definitions.pb.h"

#include <iostream>

int main( int argc, char const *argv[] ) {
	ActionTracer::ActionDataPackage package;

	uint8_t	 contents	= 0b00010001;
	uint16_t identifier = 0b1110111000010001;

	package.set_device_identifier_contents( identifier << 8 | contents );
	package.add_data( 12.3 );
	package.add_data( 3.56 );
	package.add_data( 2.3 );
	package.add_data( 1.7 );

	std::cout << package.ShortDebugString() << std::endl;
	return 0;
}
