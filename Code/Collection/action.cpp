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

#include <iostream>
#include <stddef.h>

#ifdef ON_PI
	#include <wiringPi.h>
	#include <wiringPiI2C.h>
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
	deselect_action_device( pin );
}

void ActionTracer::clear_screen() {
	printf( "%s\n", "\033[2J" );
}

void ActionTracer::print_status() {
	// Printing the live map showing what devices have been discovered
	printf( "%s\n%s\n%s\n", live_map_l, live_map_d.c_str(), live_map_l );
}

void ActionTracer::show_main_menu() {
	print_status();

	printf( "%s\n\n", "Please select an option from below:" );

	size_t p = 0;
	printf( "\t %2i. %s\n", ++p, "Discover connected devices" );
	printf( "\t %2i. %s\n", ++p, "Calibrate connected device(s)" );
	printf( "\t %2i. %s\n", ++p, "Create/Export setup file" );
	printf( "\t %2i. %s\n", ++p, "Export calibration file" );
	printf( "\t %2i. %s\n", ++p, "Initialize active devices" );
	printf( "\t %2i. %s\n", ++p, "" );
	printf( "\t %2i. %s\n", ++p, "Test all Action Device GPIO" );
}

int ActionTracer::calibrate_devices() {
	// Add question sction seeing if all devices should be done or specific device.
	int c = 0;
	for( int i = 0; i < ActionTracer::num_action_devices; i++ ) {
		if( live_map[i] ) {
			calibrate_device( get_action_tracer_device( i ) );
			c++;
		}
	}
	return c;
}

void ActionTracer::test_all_gpio() {
	silence_tracers();
	for( int i = 0; i < ActionTracer::num_action_devices; i++ ) {
		select_action_device( i );
		printf( "Currentley ACT %i is selected. This is WiringPi Pin %i\n", i, ActionTracer::get_action_tracer_device( i ) );
		std::cin.get();
		deselect_action_device( i );
	}
}

void ActionTracer::select_action_device( int action_device_number ) {
#ifdef ON_PI
	digitalWrite( ActionTracer::get_action_tracer_device( action_device_number ), HIGH );
#endif
}

void ActionTracer::deselect_action_device( int action_device_number ) {
#ifdef ON_PI
	digitalWrite( ActionTracer::get_action_tracer_device( action_device_number ), LOW );
#endif
}

void ActionTracer::calibrate_device( int device_number ) {
	obtain_offsets( ActionTracer::get_pi_location( device_number ) );
}

int ActionTracer::initialize_devices() {
	// Add question section seeing if all devices should be done or specific device.
	int c = 0;
	for( int i = 0; i < ActionTracer::num_action_devices; i++ ) {
		if( live_map[i] ) {
			initialize_device( get_action_tracer_device( i ) );
			c++;
		}
	}
	return c;
}

void ActionTracer::initialize_device( int device_number ) {
	obtain_offsets( ActionTracer::get_pi_location( device_number ) );
}

int ActionTracer::scan_i2c_for_tracers() {
	printf( "Scanning for all connected Action Tracer devices [%i - %i]\n", 0, 12 );
	// Get all the devices down to no noise
	silence_tracers();

	// Store results of each check.
	int alive = 0;

	int life_map[ActionTracer::num_action_devices] = { 0 };
	live_map_d									   = "    |";
#ifdef ON_PI
	wiringPiSetup();
#endif
	for( size_t i = 0; i < ActionTracer::num_action_devices; i++ ) {
		int WiPi_pin_number = ActionTracer::get_pi_location( i );
#ifdef ON_PI
		pinMode( WiPi_pin_number, INPUT );
		select_action_device( WiPi_pin_number );
		life_map[i] = wiringPiI2CSetup( 0x58 );
		deselect_action_device( WiPi_pin_number );
#endif

		if( life_map[i] != -1 ) {
			printf( "Action Device %i is detected. Returned code: %i\n", i, life_map[i] );
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
		int r = -1;

		std::cin >> r;

		if( r > 10 && r <= 0 ) {
			continue;
		}

		switch( r ) {
			case 1:
				printf( "You have chosen %i - Discover connected devices.\n", r );
				ActionTracer::scan_i2c_for_tracers();

				break;
			case 2:
				printf( "You have chosen %i - Calibrate connected devices.\n", r );
				ActionTracer::calibrate_devices();
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				printf( "You have chosen %i - Initialize active devices.\n", r );
				ActionTracer::initialize_devices();
				break;
			case 7:
				printf( "You have chosen %i - Test all Action Device GPIO.\n", r );
				ActionTracer::test_all_gpio();
				break;
			default:
				continue;
				break;
		}
	}
	return 0;
}