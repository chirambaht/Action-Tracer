#include "hr_algorithm.h"
#include "max30102.h"

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>

unsigned int millis( void );
void		 delay( unsigned int duration );
uint8_t		 data_buffer[512];
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

				if( !( ch_hr_valid && ch_spo2_valid ) || ( n_heart_rate > 180 || n_heart_rate < 30 ) ) {
					continue;
				}

				printf( "red = %d, ir = %d, ", aun_red_buffer[i], aun_ir_buffer[i] );

				printf( "HR = %d (%d), SP02 = %d (%d)\n", n_heart_rate, ch_hr_valid, n_spo2, ch_spo2_valid );
				delay( 30 );
			}

			maxim_heart_rate_and_oxygen_saturation( aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid );
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