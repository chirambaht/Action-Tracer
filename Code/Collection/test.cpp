

#include "MAX30102.h"
#include "MPU6050.h"
#include "heartRate.h"

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

const uint8_t RATE_SIZE		   = 10;	// Increase this for more averaging. 4 is good.
uint8_t		  rates[RATE_SIZE] = { 0 }; // Array of heart rates
uint8_t		  rateSpot		   = 0;
long		  lastBeat		   = 0; // Time at which the last beat occurred

float beatsPerMinute;
int	  beatAvg;

int main( int argc, char const *argv[] ) {
	start = std::chrono::steady_clock::now();

	printf( "This is the MAX30102%sTest\n", " " );

	MAX30102 *dev = new MAX30102(); // Body temperature
	MPU6050	*mp  = new MPU6050();	// Outside temperature

	mp->initialize();

	if( !mp->getTempSensorEnabled() ) {
		mp->setTempSensorEnabled( true );
	}

	printf( "MPU6050 Temperature Ready!\n" );

	if( !dev->begin( MAX30102_ADDRESS ) ) {
		printf( "Error! Something went wrong" );
		while( 1 ) {
		}
	}
	dev->setup();
	dev->setPulseAmplitudeRed( 0x0A ); // Turn Red LED to low to indicate sensor is running

	printf( "Temp enable first: \n" );
	dev->enableDIETEMPRDY();

	// Wait for finger to be placed on the sensor
	wait_for_beat( dev );
	// Finger dertected
	dev->setPulseAmplitudeRed( 0x0A ); // Turn Red LED to low to indicate sensor is running

	for( ;; ) {
		float temp = dev->readTemperature();
		float tt   = ( mp->getTemperature() / 340 ) + 36.53;
		printf( "Body: %5.3f \t Outside: %5.3f\n", temp, tt );

		int32_t ir_val = dev->getIR();

		if( checkForBeat( ir_val ) ) {
			// There is a beat
			long delta = millis() - lastBeat;
			lastBeat   = millis();

			beatsPerMinute = 60.0 / ( delta / 1000.0 );

			if( beatsPerMinute < 255 && beatsPerMinute > 20 ) {
				rates[rateSpot++] = ( uint8_t ) beatsPerMinute; // Store this reading in the array
				rateSpot %= RATE_SIZE;							// Wrap variable

				// Take average of readings
				beatAvg = 0;
				for( uint8_t x = 0; x < RATE_SIZE; x++ )
					beatAvg += rates[x];
				beatAvg /= RATE_SIZE;
			}
		} else {
			wait_for_beat( dev );
		}

		printf( "Heart rate: %d." );

		delay( 50 );
	}

	return 0;
}

void wait_for_beat( MAX30102 *device ) {
	uint32_t ir_val = device->getIR();
	while( ir_val < 20000 ) {
		device->setPulseAmplitudeRed( 0x0A ); // Turn Red LED to low to indicate sensor is running
		delay( 500 );
		device->setPulseAmplitudeRed( 0x0A ); // Turn Red LED to low to indicate sensor is running
		delay( 500 );
		ir_val = device->getIR();
	}
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