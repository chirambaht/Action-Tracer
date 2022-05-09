

#include "algorithm.h"
#include "max30102.h"

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>

void wait_for_beat();

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

#define MAX_BRIGHTNESS 255
auto start = std::chrono::steady_clock::now();

uint32_t aun_ir_buffer[100];  // infrared LED sensor data
uint32_t aun_red_buffer[100]; // red LED sensor data
int32_t	 n_ir_buffer_length;  // data length
int32_t	 n_spo2;			  // SPO2 value
int8_t	 ch_spo2_valid;		  // indicator to show if the SPO2 calculation is valid
int32_t	 n_heart_rate;		  // heart rate value
int8_t	 ch_hr_valid;		  // indicator to show if the heart rate calculation is valid
uint8_t	 uch_dummy;

int main( int argc, char const *argv[] ) {
	start = std::chrono::steady_clock::now();

	maxim_max30102_reset();

	maxim_max30102_read_reg( REG_INTR_STATUS_1, &uch_dummy ); // Reads/clears the interrupt status register

	maxim_max30102_init(); // initialize the MAX30102

	for( ;; ) {
		uint32_t un_min, un_max, un_prev_data, un_brightness; // variables to calculate the on-board LED brightness that reflects the heartbeats
		int32_t	 i;
		float	 f_temp;

		un_brightness = 0;
		un_min		  = 0x3FFFF;
		un_max		  = 0;

		n_ir_buffer_length = 100; // buffer length of 100 stores 4 seconds of samples running at 25sps

		// read the first 100 samples, and determine the signal range
		for( i = 0; i < n_ir_buffer_length; i++ ) {
			// wait until the interrupt pin asserts
			delay( 1 );

			maxim_max30102_read_fifo( ( aun_red_buffer + i ), ( aun_ir_buffer + i ) ); // read from MAX30102 FIFO

			if( un_min > aun_red_buffer[i] )
				un_min = aun_red_buffer[i]; // update signal min
			if( un_max < aun_red_buffer[i] )
				un_max = aun_red_buffer[i]; // update signal max

			printf( "red = %d, ir = %d\n", aun_red_buffer[i], aun_ir_buffer[i] );
		}

		un_prev_data = aun_red_buffer[i];
		// calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
		maxim_heart_rate_and_oxygen_saturation( aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid );

		// Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
		while( 1 ) {
			i	   = 0;
			un_min = 0x3FFFF;
			un_max = 0;

			// dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
			for( i = 25; i < 100; i++ ) {
				aun_red_buffer[i - 25] = aun_red_buffer[i];
				aun_ir_buffer[i - 25]  = aun_ir_buffer[i];

				// update the signal min and max
				if( un_min > aun_red_buffer[i] )
					un_min = aun_red_buffer[i];
				if( un_max < aun_red_buffer[i] )
					un_max = aun_red_buffer[i];
			}

			// take 25 sets of samples before calculating the heart rate.
			for( i = 75; i < 100; i++ ) {
				un_prev_data = aun_red_buffer[i - 1];
				delay( 1 );
				maxim_max30102_read_fifo( ( aun_red_buffer + i ), ( aun_ir_buffer + i ) );

				// calculate the brightness of the LED
				if( aun_red_buffer[i] > un_prev_data ) {
					f_temp = aun_red_buffer[i] - un_prev_data;
					f_temp /= ( un_max - un_min );
					f_temp *= MAX_BRIGHTNESS;
					f_temp = un_brightness - f_temp;
					if( f_temp < 0 )
						un_brightness = 0;
					else
						un_brightness = ( int ) f_temp;
				} else {
					f_temp = un_prev_data - aun_red_buffer[i];
					f_temp /= ( un_max - un_min );
					f_temp *= MAX_BRIGHTNESS;
					un_brightness += ( int ) f_temp;
					if( un_brightness > MAX_BRIGHTNESS )
						un_brightness = MAX_BRIGHTNESS;
				}

				// send samples and calculation result to terminal program through UART

				printf( "red = %d, ir = %d, ", aun_red_buffer[i], aun_ir_buffer[i] );

				printf( "HR = %d (%d), SP02 = %d (%d)\n", n_heart_rate, ch_hr_valid, n_spo2, ch_spo2_valid );
			}

			maxim_heart_rate_and_oxygen_saturation( aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid );
		}
	}

	return 0;
}

// void wait_for_beat( MAX30102 *device ) {
// 	uint32_t ir_val = device->getIR(), red_val = device->getRed();
// 	printf( "Waiting for a finger to be detected\n" );
// 	printf( " IR value %6d \t Red Value: %6d\n", ir_val, red_val );
// 	while( ir_val < 20000 ) {
// 		device->setPulseAmplitudeRed( 0xFF ); // Turn Red LED to high to indicate sensor is running
// 		delay( 250 );
// 		red_val = device->getRed();
// 		delay( 250 );
// 		device->setPulseAmplitudeRed( 0 ); // Turn Red LED to low to indicate sensor is running
// 		delay( 500 );
// 		ir_val = device->getIR();
// 		printf( " IR value %d \t Red Value: %d\n", ir_val, red_val );
// 	}
// 	device->setPulseAmplitudeRed( 0 ); // Turn Red LED to low to indicate sensor is running
// }

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