

#include "MAX30102.h"
#include "MPU6050.h"
#include "spo2_algorithm.h"

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

auto	 start = std::chrono::steady_clock::now();
uint32_t irBuffer[100];	 // infrared LED sensor data
uint32_t redBuffer[100]; // red LED sensor data

int32_t bufferLength;	// data length
int32_t spo2;			// SPO2 value
int8_t	validSPO2;		// indicator to show if the SPO2 calculation is valid
int32_t heartRate;		// heart rate value
int8_t	validHeartRate; // indicator to show if the heart rate calculation is valid

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

	dev->enableDIETEMPRDY();
	dev->setPulseAmplitudeRed( 0x0A ); // Turn Red LED to low to indicate sensor is running
	dev->setPulseAmplitudeGreen( 0 );  // Turn off Green LED

	for( ;; ) {
		bufferLength = 100; // buffer length of 100 stores 4 seconds of samples running at 25sps

		// read the first 100 samples, and determine the signal range
		for( uint8_t i = 0; i < bufferLength; i++ ) {
			while( dev->available() == false ) // do we have new data?
				dev->check();				   // Check the sensor for new data

			redBuffer[i] = dev->getRed();
			irBuffer[i]	 = dev->getIR();
			dev->nextSample(); // We're finished with this sample so move to next sample

			printf( "Red = %d \t IR = %d", redBuffer[i], irBuffer[i] );
		}

		// calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
		maxim_heart_rate_and_oxygen_saturation( irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate );

		// Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
		while( 1 ) {
			// dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
			for( uint8_t i = 25; i < 100; i++ ) {
				redBuffer[i - 25] = redBuffer[i];
				irBuffer[i - 25]  = irBuffer[i];
			}

			// take 25 sets of samples before calculating the heart rate.
			for( uint8_t i = 75; i < 100; i++ ) {
				while( dev->available() == false ) // do we have new data?
					dev->check();				   // Check the sensor for new data

				redBuffer[i] = dev->getRed();
				irBuffer[i]	 = dev->getIR();
				dev->nextSample(); // We're finished with this sample so move to next sample

				// send samples and calculation result to terminal program through UART

				printf( "Red = %i\tIR = %i\tHR = %i\tHRvalid = %i\tSP02 = %i\tSP02valid = %i\n", redBuffer[i], irBuffer[i], heartRate, validHeartRate, spo2, validSPO2 );
			}

			// After gathering 25 new samples recalculate HR and SP02
			maxim_heart_rate_and_oxygen_saturation( irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate );
		}
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