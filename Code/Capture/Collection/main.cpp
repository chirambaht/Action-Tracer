// This is the main file that will be used to run the program for data
// collection from the 3 IMU's and send them to the server as is necesarry.
#include "debug_printer.h"
#include "main.h"

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

	*communicator = new Packager( _address, PORT ); // Initialize the communicator that will send data packets to the server
	*communicator->set_debug( debug_value );

	// for( size_t i = 0; i < _sensors; i++ ) {
	// 	body_sensor[i] = new TracePoint( "Body p", 2 );
	// }

	for( auto i = bodysensors.begin(); i < bodysensors.end(); std::advance( i, 1 ) ) {
		*i = new TracePoint( "Body p", 2 );
	}
}

/*
  Main work loop for all the code. This will always run every cycle.
*/
void loop() {
	float data_package[_sensors * 4];

	// for( size_t i = 0; i < _sensors; i++ ) {
	// 	float *body = body_sensor[i]->read_data( 1 );
	// 	for( size_t j = 0; j < 4; j++ ) {
	// 		data_package[j + ( i * 4 )] = *body;
	// 		body++;
	// 	}
	// }
	int i = 0;
	for( auto dev : bodysensors ) {
		float *body = dev.read_data( 1 );

		for( size_t j = 0; j < 4; j++ ) {
			data_package[j + ( i * 4 )] = *body;
			body++;
		}
		++i;
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
