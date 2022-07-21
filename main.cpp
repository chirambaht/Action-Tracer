// This is the main file that will be used to run the program for data
// collection from the 3 IMU's and send them to the server as is necessary.

#include "main.h"

#include "action_pi.hpp"
#include "debug_printer.h"

#ifdef TAKE_ARGUMENTS
	#include <cxxopts.hpp>
	#include <string>
#endif

#include <signal.h>
#include <stdlib.h>

#ifdef ON_PI
	#include <wiringPi.h>
#endif

using namespace ActionTracer;

#ifdef TAKE_ARGUMENTS
cxxopts::Options options( "Action Tracer", "This program runs a given number of MPU6050 IMU's and sends the data packets via UDP." );
#endif

#ifdef ON_PI
PI_THREAD( network_watcher ) {
	printf( "Network Management Thread starting...\n" );
	communicator = new Packager( PORT ); // Initialize the communicator that will send data packets to the server
	// communicator->_number_of_devices = _sensors;
	communicator->socket_setup();

	// while( communicator->_clients_connected() < 1 ) {
	// 	printf( "clients: %d\n", communicator->_clients_connected() );
	// }
	// printf( "At least 1 client connected: %d\n", communicator->_clients_connected() );

	for( ;; ) {
		if( send_ready == false ) {
			communicator->run_socket_manager();
		}
	}

	printf( "For some reason, this has run: manager" );
}

PI_THREAD( network_sender ) {
	printf( "Sending thread starting...\n" );
	for( ;; ) {
		while( !send_ready ) {
			continue;
		}
		piLock( 1 );

		// Send packet
		communicator->send_to_connected_devices();
		send_ready = false;

		piUnlock( 1 );
	}
	printf( "For some reason, this has run: sender" );
}
#endif

void turn_off_all_devices() {
	for( int dev = 0; dev < num_action_devices; dev++ ) {
#ifdef ON_PI
		digitalWrite( get_pi_location( dev ), LOW );
#endif
	}
}

/**
 * @brief Initialise all the devices in the network. Store them in objects in main.h
 * @return 0 if success
 */
void setup() {
#ifdef ON_PI
	wiringPiSetup();
#endif

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = exit_handler;
	sigemptyset( &sigIntHandler.sa_mask );
	sigIntHandler.sa_flags = 0;

	sigaction( SIGINT, &sigIntHandler, NULL );

	// Create thread to work on networking with the packager

#ifdef ON_PI
	// Will break here if it doesn't find a TCP socket available
	if( piThreadCreate( network_watcher ) != 0 ) {
		printf( "Failed to create network worker thread\n" );
		exit( EXIT_FAILURE );
	}
	if( piThreadCreate( network_sender ) != 0 ) {
		printf( "Failed to create network worker thread\n" );
		exit( EXIT_FAILURE );
	}
#endif

#ifdef SINGLE_ACT_0
	// Init all the devices as the IMU on the Pi Connector
	printf( "Only 1 device being initialised on pin %d but it will be copied across the sensor array\n", get_pi_location( 0 ) );
	body_sensor[0] = new TracePoint( 0, get_pi_location( 0 ) );
	body_sensor[1] = body_sensor[0];
	body_sensor[2] = body_sensor[0];
	_sensors_init  = 3;
#else
	for( size_t i = 0; i < _sensors; i++ ) {
		printf( "New device initialising on WiringPi pin %d aka ACT_%d\n", get_pi_location( i + 1 ), i + 1 );
		body_sensor[i] = new TracePoint( i, get_pi_location( i + 1 ) ); // offset by 1 is to ensure we are starting at ACT_DEVICE_1

		_sensors_init++;
	}
#endif
}

void exit_handler( int s ) {
	printf( "\nTurning off all devices...\n" );
	turn_off_all_devices();
	printf( "Cleaning up the communicator\n" );
	communicator->dump_vars();
	printf( "Closing sockets\n" );
	communicator->close_all_sockets();
	exit( 1 );
}

/**
 * @brief Main work loop for all the code. This will always run every cycle.
 * @return Nothing
 */
void loop() {
	float *body;
#ifdef ON_PI
	loop_start = millis();
	while( millis() - loop_start < ( WAIT_TIME / 2 ) ) {
		continue;
	}
	loop_start = millis();
#endif
#ifndef SINGLE_ACT_0
	for( size_t i = 0; i < _sensors; i++ ) {
		body = body_sensor[i]->read_data( 1 );
		for( size_t j = 0; j < 19; j++ ) {
			data_package[j] = *body++;
		}
		communicator->load_packet( data_package, i + 1, 19 );
	}
#else
	body = body_sensor[0]->read_data( 1 );
	for( size_t j = 0; j < 19; j++ ) {
		data_package[j] = *body++;
	}
	communicator->load_packet( data_package, 1, 19 );
	communicator->load_packet( data_package, 2, 19 );
	communicator->load_packet( data_package, 3, 19 );
#endif
#ifdef ON_PI
	piLock( 1 );
	send_ready = true;

	while( millis() - loop_start < ( WAIT_TIME / 2 ) ) {
		continue;
	}
	piUnlock( 1 );
#endif
}

/**
 * @brief Main method that schedules the main tasks before and after data collection.
 *
 * @param argc
 * @param argv
 * @return int
 */
int main( int argc, char const *argv[] ) {
#ifdef TAKE_ARGUMENTS
	options.add_options()( "f,file", "Define variables using a file. If a file is given, all other given parameters will be overwritten.", cxxopts::value<std::string>()->default_value( "" ) );
	options.add_options()( "h,help", "Print usage" );
	options.add_options()( "t,tracepoints", "Number of body devices being used on the body", cxxopts::value<int>()->default_value( "0" ) );

	auto result = options.parse( argc, argv );

	if( result.count( "help" ) ) {
		printf( "%s", options.help().c_str() );
		exit( 0 );
	}

	_sensors = result["tracepoints"].as<int>();

	if( result.count( "file" ) ) {
		// TODO: Read CSV file for parameters for each item
	}
#else

	_sensors = 3;
	printf( "Devices connected: %d\n", _sensors );
#endif

	// TODO: Run a new setup method that accounts for debug, custom tps, files and
	printf( "Running basic setup routine\n" );
	setup();

	printf( "\nSetup Complete! Running now\n\n" );
	while( 1 ) {
		loop();
	}

	printf( "For some reason, this has run: main" );
	return 0;
}