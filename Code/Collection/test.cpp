

#include "MAX30102.h"
#include "MPU6050.h"

#include <chrono>
#include <cstdio>

#define DEBUG

#ifndef _DEBUG_PRINTER_H_
	#define _DEBUG_PRINTER_H_

	#ifdef DEBUG
		#define debugPrint( ... )	 printf( __VA_ARGS__ )
		#define debugPrintln( ... )	 printf( __VA_ARGS__ )
		#define fdebugPrint( ... )	 fprintf( __VA_ARGS__ )
		#define fdebugPrintln( ... ) fprintf( __VA_ARGS__ )
	#else
		#define debugPrint( ... )
		#define debugPrintln( ... )
		#define fdebugPrint( ... )
		#define fdebugPrintln( ... )
	#endif

#endif

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
	dev->softReset();
	uint8_t *ll = dev->readAllRegisters();
	int		 p	= 0;

	for( uint8_t c = 0; c < 0x100; c += 1 ) {
		if( c == 0x0B || c == 0x0E || c == 0x0F || c == 0x10 || ( c <= 0x1E && c >= 0x13 ) || ( c < 0xFE && c >= 0x22 ) ) {
			continue;
		}
		printf( "Register 0x%2x - %2x\n", c, dev->_all_reg[p++] );
		if( p == 20 ) {
			uint8_t a = dev->readPartID(), b = dev->getRevisionID();
			printf( "Register 0xFE - 0x%2x\n", b );
			printf( "Register 0xFF - 0x%2x\n", a );
			break;
		}
	}

	dev->setup();

	dev->enableDIETEMPRDY();
	dev->setPulseAmplitudeRed( 0x0A ); // Turn Red LED to low to indicate sensor is running

	ll = dev->readAllRegisters();
	p  = 0;

	for( uint8_t c = 0; c < 0x100; c += 1 ) {
		if( c == 0x0B || c == 0x0E || c == 0x0F || c == 0x10 || ( c <= 0x1E && c >= 0x13 ) || ( c < 0xFE && c >= 0x22 ) ) {
			continue;
		}
		printf( "Register 0x%2x - 0x%2x\n", c, dev->_all_reg[p++] );
		if( p == 20 ) {
			break;
		}
	}

	for( ;; ) {
		// float temp = dev->readTemperature();
		// float tt   = ( mp->getTemperature() / 340 ) + 36.53;
		// printf( "Temp: %5.3f vs %5.3f\n", temp, tt );
		uint32_t r = dev->getRed(), i = dev->getIR();
		printf( "R[%i]\tIR[%i]\n", r, i );

		delay( 250 );
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