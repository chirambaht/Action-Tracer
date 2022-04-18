// This will be the interactive shell to get the different segments of code working.

/*
The goals of this section is to write code that can:
1. Take note of all the devices that are linked through I2C scan
2. Callibrate all the devices and load their configurations instantly
3. Take readings, consistently from all devices
4. Save config or reading data as requested.


Remember, on is when we're talking to device 0x69
*/

#include "action.h"

#include "IMU_zero.hpp"
#include "action_pi.hpp"
#include "tracer_point.h"

#include <iostream>
#include <stddef.h>

#ifdef ON_PI
	#include <wiringPi.h>
	#ifndef HIGH
		#define HIGH 1
	#endif
	#ifndef LOW
		#define LOW 0
	#endif
#endif

void ActionTracer::print_title() {
	printf( "%s\n", headline_s );

	printf( "%s\n", headline_1 );
	printf( "%s\n", headline_2 );
	printf( "%s\n", headline_3 );
	printf( "%s\n", headline_4 );

	printf( "%s\n", headline_s );
}

void ActionTracer::silence_tracers() {
	for( size_t i = 0; i < ActionTracer::num_action_devices; i++ ) {
		silence_tracer( ActionTracer::get_pi_location( i ) );
	}
}

void ActionTracer::silence_tracer( int pin ) {
#ifdef ON_PI
	digitalWrite( pin, HIGH );
#endif
}

void ActionTracer::clear_screen() {
	printf( "%s\n", "\033[2J" );
}

void ActionTracer::print_status() {
	// Printing the live map showing what devices have been discovered
	printf( "%s\n%s\n%s", live_map_l, live_map_d.c_str(), live_map_l );
}

void ActionTracer::show_main_menu() {
	print_status();

	printf( "%s\n\n", "Please select an option from below:" );

	size_t p = 0;
	printf( "\t %2i. %s\n", ++p, "Discover connected devices" );
	printf( "\t %2i. %s\n", ++p, "Calibrate connected device(s)" );
	printf( "\t %2i. %s\n", ++p, "Create/Export setup file" );
	printf( "\t %2i. %s\n", ++p, "Export calibration file" );
	printf( "\t %2i. %s\n", ++p, "" );
	printf( "\t %2i. %s\n", ++p, "" );
	printf( "\t %2i. %s\n", ++p, "" );
}

int ActionTracer::calibrate_devices() {
	for( int i = 0; i < ActionTracer::num_action_devices; i++ ) {
		if( live_map[i] ) {
			calibrate_device( get_action_tracer_device( i ) );
		}
	}
}

void ActionTracer::calibrate_device( int device_number ) {
	obtain_offsets( ActionTracer::get_pi_location( device_number ) );
}

int ActionTracer::scan_i2c_for_tracers() {
	// Get all the devices down to no noise
	silence_tracers();
	int res;

	// Store results of each check.
	int alive = 0;

	int life_map[ActionTracer::num_action_devices] = { false };
	live_map_d									   = "    |";

	for( size_t i = 0; i < ActionTracer::num_action_devices; i++ ) {
#ifdef ON_PI
		digitalWrite( ActionTracer::get_pi_location( i ), HIGH );
		life_map[i] = wiringPiI2CSetup( 0x69 );
		digitalWrite( ActionTracer::get_pi_location( i ), LOW );
#endif

		if( life_map[i] != -1 ) {
			alive++;
			if( i < 10 ) {
				live_map_d += "  " + std::to_string( i ) + " ";
			} else {
				live_map_d += " " + std::to_string( i ) + " ";
			}
			live_map_d += "|";
		}
	}
	return alive;
}

int main( int argc, char const *argv[] ) {
	ActionTracer::print_title();

	for( ;; ) {
		ActionTracer::show_main_menu();
		int r;
		std::cin >> r;

		switch( r ) {
			case 1:
				ActionTracer::scan_i2c_for_tracers();
				break;
			case 2:
				ActionTracer::calibrate_devices();
				break;
			default:
				continue;
				break;
		}
	}
	return 0;
}