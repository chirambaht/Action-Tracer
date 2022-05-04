

#include "MAX30102.h"

#include <chrono>
#include <cstdio>

auto start = std::chrono::steady_clock::now();

int main( int argc, char const *argv[] ) {
	start = std::chrono::steady_clock::now();

	printf( "This is the MAX30102%sTest\n", " " );

	MAX30102 *dev = new MAX30102();

	if( !dev->begin( MAX30102_ADDRESS ) ) {
		printf( "Error! Something went wrong" );
		while( 1 ) {
		}
	}

	printf( "Got this part ID: %x\n Now we shall get the heart rate\n\n", id );

	printf( "Temp enable first: \n" );

	dev->setup();

	float temp = dev->readTemperature();
	printf( "Temp: %f\n", temp );

	for( ;; ) {
	}

	return 0;
}

unsigned int millis( void ) {
	auto end = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();
}

void delay( unsigned int duration ) {
	auto t	 = std::chrono::steady_clock::now();
	auto end = std::chrono::steady_clock::now();
	do {
		end = std::chrono::steady_clock::now();
	} while( std::chrono::duration_cast<std::chrono::milliseconds>( end - t ).count() < duration );
}