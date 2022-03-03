// This is the main file that will be used to run the program for data
// collection from the 3 IMU's and send them to the server as is necessary.

#include "main.h"

#include "debug_printer.h"

#ifdef TAKE_ARGUMENTS
	#include <cxxopts.hpp>
#endif

#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <math.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <thread>
#include <unistd.h>

#ifdef ON_PI
	#include <wiringPi.h>
#endif
/// Outline

using namespace ActionTracer;

#ifdef TAKE_ARGUMENTS
cxxopts::Options options( "Action Tracer", "This program runs a given number of MPU6050 IMU's and sends the data packets via UDP." );
#endif

/**
 * @brief Initialise all the devices in the network. Store them in objects in main.h
 * @param debug_value sets the debug status for the setup routine.
 * @return 0 if success
 */
void setup( int debug_value = 0 ) {
#ifdef ON_PI
	wiringPiSetup();
#endif
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = exit_handler;
	sigemptyset( &sigIntHandler.sa_mask );
	sigIntHandler.sa_flags = 0;

	sigaction( SIGINT, &sigIntHandler, NULL );

	communicator = new Packager( _address, PORT ); // Initialize the communicator that will send data packets to the server
	communicator->set_debug( debug_value );
	communicator->save_enable( true );
	communicator->init_tcp();
	// communicator->init_udp();

#ifdef ARRAY_SOLUTION
	for( size_t i = 0; i < _sensors; i++ ) {
		body_sensor[i] = new TracePoint( "Body p", get_pi_location( i ) );
	}
#endif
}

void exit_handler( int s ) {
	if( communicator->save_status() )
		communicator->close_file();
	communicator->close_socket();
	exit( 1 );
}

/**
 * @brief Main work loop for all the code. This will always run every cycle.
 * @return Nothing
 */
void loop() {
#ifdef ARRAY_SOLUTION
	float *body;
	for( size_t i = 0; i < _sensors; i++ ) {
		body = body_sensor[i]->read_data( 1 );
		for( size_t j = 0; j < 4; j++ ) {
			data_package[j] = *body;
			body++;
		}
		( communicator->load_packet( data_package, 4 ) );
	}
#endif

/**
 * @brief This allows tracking of frames sent to the network against the ones collected from the device.
 */
#ifdef COUNT_FRAMES
	int failed_send_ = communicator->send_packet();
	_packets_collected_per_second++;

	uint32_t t = millis();

	if( !failed_send_ ) {
		_packets_sent_per_second++;
	}

	if( ( t - _start_time ) >= 1000 ) {
		_seconds_since_start++;
		_packets_collected += _packets_collected_per_second;
		_packets_sent += _packets_sent_per_second;

		_average_packets_collected = _packets_collected / _seconds_since_start;
		_average_packets_sent	   = _packets_sent / _seconds_since_start;

		printf( "|| %4d | %5d | %5.1f | %5d | %5.1f ||\t%10d\n", _seconds_since_start, _packets_collected_per_second, _average_packets_collected, _packets_sent_per_second, _average_packets_sent, t );

		_packets_collected_per_second = 0;
		_packets_sent_per_second	  = 0;
		_start_time					  = millis();
	}
#else
	// Send packet
	communicator->send_packet();
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
	options.add_options()( "a,address", "Address to send UDP packets to", cxxopts::value<std::string>()->default_value( "127.0.0.1" ) );
	options.add_options()( "d,debug", "Enable debugging", cxxopts::value<bool>()->default_value( "false" ) );
	options.add_options()( "f,file", "Define variables using a file. If a file is given, all other given parameters will be overwritten.", cxxopts::value<std::string>()->default_value( "" ) );
	options.add_options()( "h,help", "Print usage" );
	options.add_options()( "t,tracepoints", "Number of body devices being used on the body", cxxopts::value<int>()->default_value( "0" ) );

	auto result = options.parse( argc, argv );

	if( result.count( "help" ) ) {
		printf( "%s", options.help().c_str() );
		exit( 0 );
	}

	_address = result["address"].as<std::string>();

	_debug	 = result["debug"].as<bool>();
	_sensors = result["tracepoints"].as<int>();

	if( result.count( "file" ) ) {
		// TODO: Read CSV file for parameters for each item
	}
#endif

#ifndef TAKE_ARGUMENTS
	// int			  number_of_lines = 0;
	// std::string	  line;
	// std::ifstream my_file( "pointers.csv" );

	// while( std::getline( my_file, line ) )
	// 	++number_of_lines;

	#ifndef SEND_ADDRESS
	_address = "192.168.0.149";
	#else
	_address = SEND_ADDRESS;
	#endif
	_sensors = 3;
	_debug	 = false;
	printf( "Devices connected: %d\n", _sensors );
#endif

	// TODO: Run a new setup method that accounts for debug, custom tps, files and addresses
	setup( _debug );

#ifdef COUNT_FRAMES
	printf( "Start time: %d\n\n", _start_time );
	printf( "|| %4s | %5s | %5.1s | %5s | %5.1s ||\n", "t(s)", "pc/s", "apc", "ps/s", "aps" );
	_start_time = millis();
#endif

	while( 1 ) {
#ifdef TIMING
		uint32_t tstart, tend;
		tstart = millis();
#endif
		loop();
#ifdef TIMING
		tend = millis();
		printf( "It took %dms to run the loop.\n", tend - tstart );
#endif
	}
	return 0;
}