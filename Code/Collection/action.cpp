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

#include "action_pi.hpp"
#include "tracer_point.h"

#include <iostream>
#include <stddef.h>

#ifdef ON_PI
	#include <wiringPi.h>
#endif

void ActionTracer::print_title() {
	printf( "%s\n", headline_s );
	printf( "%s\n", headline_0 );
	printf( "%s\n", headline_1 );
	printf( "%s\n", headline_2 );
	printf( "%s\n", headline_3 );
	printf( "%s\n", headline_4 );
	printf( "%s\n", headline_5 );
	printf( "%s\n", headline_6 );
	printf( "%s\n", headline_7 );
	printf( "%s\n", headline_8 );
	printf( "%s\n", headline_s );
}

int main( int argc, char const *argv[] ) {
	ActionTracer::print_title();

	for( ;; ) {
		// This is the action loop that waits for commands and works
	}
	return 0;
}

void ActionTracer::silence_tracers() {
	for( size_t i = 0; i < ActionTracer::num_action_devices; i++ ) {
		silence_tracer( ActionTracer::get_pi_location( i ) );
	}
}

void ActionTracer::clear_screen() {
	printf( "%s\n", "\033[2J" );
}

void ActionTracer::show_main_menu() {
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

void ActionTracer::silence_tracer( int pin ) {
#ifdef ON_PI
	digitalWrite( pin, 1 );
#endif
}

void get_offsets( int device_number, bool printed = false ) {
}

int ActionTracer::scan_i2c_for_tracers() {
	// Get all the devices down to no noise
	silence_tracers();
	int res;

	// Store results of each check.
	int alive = 0;

	int life_map[ActionTracer::num_action_devices] = { false };

	for( size_t i = 0; i < ActionTracer::num_action_devices; i++ ) {
#ifdef ON_PI
		life_map[i] = wiringPiI2CSetup( int devId );
#endif
		if( life_map[i] != -1 ) {
			alive++;
		}
	}
	return alive;
}