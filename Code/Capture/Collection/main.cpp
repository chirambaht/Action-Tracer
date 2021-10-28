// This is the main file that will be used to run the program for data
// collection from the 3 IMU's and send them to the server as is necesarry.
#include "main.h"

#include "debug_printer.h"

#include <cxxopts.hpp>
#include <dirent.h>
#include <filesystem>
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
#include <wiringPi.h>
/// Outline

using namespace ActionTracer;

cxxopts::Options options( "Action Tracer", "This program runs a given number of MPU6050 IMU's and sends the data packets via UDP." );

/*
    Initialise all the devices in the network. Store them in objects in main.h
*/

void setup( int debug_value = 0 ) {
	wiringPiSetup();

	communicator = new Packager( "127.0.0.1", PORT ); // Initialize the communicator that will send data packets to the server
	communicator->set_debug( debug_value );
	// The following are the sensors being used to obtain data from the body.

	// TracePoint *a = new TracePoint( "Left Shoulder", 2 );
	// TracePoint *b = new TracePoint( "Left Forearm", 3 );
	// TracePoint *c = new TracePoint( "Left Hand", 4 );

	// body_sensor[0] = a;
	// body_sensor[1] = b;
	// body_sensor[2] = c;

	for( size_t i = 0; i < _sensors; i++ ) {
		TracePoint *temp = new TracePoint( "Body p", 2 );
		temp->set_debug( debug_value );
		body_sensor[i] = temp;
	}
}

/*
  Main work loop for all the code. This will always run every cycle.
*/
void loop() {
	float data_package[_sensors * 4];

	for( size_t i = 0; i < _sensors; i++ ) {
		float *body = body_sensor[i]->read_data( 1 );
		for( size_t j = 0; j < 4; j++ ) {
			data_package[j + ( i * 4 )] = *body;
			body++;
		}
	}

	communicator->send_packet( data_package, _sensors * 4 );

	usleep( LOOP_DELAY * 1000 );
}

int main( int argc, char const *argv[] ) {
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

	// TODO: Run a new setup method that accounts for debug, custom tps, files and addresses
	setup( _debug );

	while( 1 ) {
		loop();
	}
	return 0;
}
