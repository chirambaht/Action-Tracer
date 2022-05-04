#include "MAX30102.h"

#include <cstdio>

int main( int argc, char const *argv[] ) {
	printf( "This is the MAX30102%sTest\n", " " );

	MAX30102 *dev = new MAX30102();

	dev->begin( MAX30102_ADDRESS );

	int id = dev->readPartID();

	printf( "Got this part ID: %x\n", id );

	return 0;
}
