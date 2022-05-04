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

auto		  start		= std::chrono::steady_clock::now();
const uint8_t RATE_SIZE = 4;	// Increase this for more averaging. 4 is good.
uint8_t		  rates[RATE_SIZE]; // Array of heart rates
uint8_t		  rateSpot = 0;
long		  lastBeat = 0; // Time at which the last beat occurred

float beatsPerMinute;
int	  beatAvg;
int	  main( int argc, char const *argv[] ) {
	  start = std::chrono::steady_clock::now();

	  printf( "This is the MAX30102%sTest\n", " " );

	  MAX30102 *dev = new MAX30102();
	  MPU6050  *mp	= new MPU6050();

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

	  dev->enableDIETEMPRDY();
	  dev->setPulseAmplitudeRed( 0x0A ); // Turn Red LED to low to indicate sensor is running
	  dev->setPulseAmplitudeGreen( 0 );	 // Turn off Green LED

	  for( ;; ) {
		  // float temp = dev->readTemperature();
		  // float tt   = ( mp->getTemperature() / 340 ) + 36.53;
		  // printf( "Temp: %5.3f vs %5.3f\n", temp, tt );
		  long irValue = dev->getIR();

		  if( checkForBeat( irValue ) == true ) {
			  // We sensed a beat!
			  long delta = millis() - lastBeat;
			  lastBeat	 = millis();

			  beatsPerMinute = 60 / ( delta / 1000.0 );

			  if( beatsPerMinute < 255 && beatsPerMinute > 20 ) {
				  rates[rateSpot++] = ( uint8_t ) beatsPerMinute; // Store this reading in the array
				  rateSpot %= RATE_SIZE;						  // Wrap variable

				  // Take average of readings
				  beatAvg = 0;
				  for( uint8_t x = 0; x < RATE_SIZE; x++ )
					  beatAvg += rates[x];
				  beatAvg /= RATE_SIZE;
			  }
		  }

		  printf( "IR= %d, BPM = %f, Avg BPM: %d.\n", irValue, beatsPerMinute, beatAvg );

		  if( irValue < 50000 )
			  printf( " No finger?\n" );

		  delay( 100 );
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