// This is the main file that will be used to run the program for data
// collection from the 3 IMU's and send them to the server as is necesarry.

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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <unistd.h>

#ifdef ON_PI
	#include <wiringPi.h>
#endif
/// Outline

using namespace ActionTracer;

#ifdef TAKE_ARGUMENTS
cxxopts::Options options( "Action Tracer", "This program runs a given number of MPU6050 IMU's and sends the data packets via UDP." );
#endif
/*
    Initialise all the devices in the network. Store them in objects in main.h
*/

void setup( int debug_value = 0 ) {
#ifdef ON_PI
	wiringPiSetup();
#endif

	communicator = new Packager( _address, PORT ); // Initialize the communicator that will send data packets to the server
	communicator->set_debug( debug_value );

#ifdef ARRAY_SOLUTION
	for( size_t i = 0; i < _sensors; i++ ) {
		body_sensor[i] = new TracePoint( "Body p", PI_ORDER[i] );
	}
#endif
#ifdef VECTOR_SOLUTION
	for( auto i = bodysensors.begin(); i < bodysensors.end(); std::advance( i, 1 ) ) {
		i = new TracePoint( "Body p", 2 );
	}
#endif
}

/*
  Main work loop for all the code. This will always run every cycle.
*/
void loop() {
#ifdef ARRAY_SOLUTION
	for( size_t i = 0; i < _sensors; i++ ) {
		float *body = body_sensor[i]->read_data( 1 );
		for( size_t j = 0; j < 4; j++ ) {
			data_package[j] = *body;
			body++;
		}
		communicator->load_packet( data_package, 4 );
	}
#endif

#ifdef VECTOR_SOLUTION
	int i = 0;
	for( auto dev : bodysensors ) {
		float *body = dev.read_data( 1 );

		for( size_t j = 0; j < 4; j++ ) {
			data_package[j + ( i * 4 )] = *body;
			body++;
		}
		++i;
	}
#endif

	// Send packet
	int gone = communicator->send_packet();
#ifdef COUNT_FRAMES
	_packets_sent++;
	uint32_t t = millis();
	// printf( "%d\n", t );
	if( ( t - _start_time ) > 1000 ) {
		_average_packets_collected = _packets_collected / _seconds_since_start + 1;
		_average_packets_sent	   = _packets_sent / _seconds_since_start + 1;

		printf( "|| %4d | %5d | %5d | %5d | %5d ||\n", _seconds_since_start, _packets_collected_per_second, _average_packets_collected, _packets_sent_per_second, _average_packets_sent );

		_seconds_since_start++;
		_packets_collected_per_second = 0;
		_packets_sent_per_second	  = 0;
		_start_time					  = millis();
	} else {
		if( !gone ) {
			_packets_sent++;
			_packets_sent_per_second++;
		}
		_packets_collected++;
		_packets_collected_per_second++;
	}
#endif
	usleep( LOOP_DELAY * 1000 );
}

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
	int			  number_of_lines = 0;
	std::string	  line;
	std::ifstream myfile( "pointers.csv" );

	while( std::getline( myfile, line ) )
		++number_of_lines;

	_address = "192.168.137.1";
	_sensors = number_of_lines;
	_debug	 = false;
	printf( "Devices connected: %d\n", _sensors );
#endif
	// TODO: Run a new setup method that accounts for debug, custom tps, files and addresses
	setup( _debug );
#ifdef COUNT_FRAMES
	_start_time = millis();
	printf( "Start time: %d\n\n", _start_time );
	printf( "|| %4s | %5s | %5s | %5s | %5s ||\n", "t(s)", "pc/s", "apc", "ps/s", "aps" );

#endif
	while( 1 ) {
		loop();
	}
	return 0;
}
