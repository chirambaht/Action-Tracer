

#include "MAX30102.h"
#include "MPU6050.h"

#include <chrono>
#include <cstdio>

auto start = std::chrono::steady_clock::now();

int main( int argc, char const *argv[] ) {
	start = std::chrono::steady_clock::now();

	printf( "This is the MAX30102%sTest\n", " " );

	MAX30102 *dev = new MAX30102();
	MPU6050	*mp  = new MPU6050();

	mp->initialize();

	if( !mp->getTempSensorEnabled() ) {
		mp->setTempSensorEnabled( true );
	}

	if( !dev->begin( MAX30102_ADDRESS ) ) {
		printf( "Error! Something went wrong" );
		while( 1 ) {
		}
	}

	printf( "Temp enable first: \n" );

	dev->setup();

	for( ;; ) {
		float temp = dev->readTemperature();
		float tt   = ( mp->getTemperature() / 340 ) + 36.53;
		printf( "Temp: %5.3f vs %5.3f\n", temp, tt );
		delay( 1000 );
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