// This is the main file that will be used to run the program for data
// collection from the 3 IMU's and send them to the server as is necesarry.
#include "debug_printer.h"
#include "main.h"

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

/*
    Initialise all the devices in the network. Store them in objects in main.h
*/

void setup() {
	wiringPiSetup();

	communicator = new Packager( SERVER_IP, PORT ); // Initialize the communicator that will send data packets to the server

	// The following are the sensors being used to obtain data from the body.

	// TracePoint *a = new TracePoint( "Left Shoulder", 2 );
	// TracePoint *b = new TracePoint( "Left Forearm", 3 );
	// TracePoint *c = new TracePoint( "Left Hand", 4 );

	// body_sensor[0] = a;
	// body_sensor[1] = b;
	// body_sensor[2] = c;

	for( size_t i = 0; i < N; i++ ) {
		TracePoint *temp = new TracePoint( "Body p", 2 );
		body_sensor[i]	 = temp;
	}

	debugPrint( "Devices connected are:\n" );
	for( size_t i = 0; i < N; i++ ) {
		debugPrint( "%2d - %s\n", i + 1, body_sensor[i]->get_name().c_str() );
	}
}

/*
  Main work loop for all the code. This will always run every cycle.
*/
void loop() {
	float data_package[N * 4];

	for( size_t i = 0; i < N; i++ ) {
		float *body = body_sensor[i]->read_data( 1 );
		for( size_t j = 0; j < 4; j++ ) {
			data_package[j + ( i * 4 )] = *body;
			body++;
		}
	}

	communicator->send_packet( data_package, N * 4 );

	usleep( LOOP_DELAY * 1000 );
}

int main( int argc, char const *argv[] ) {
	setup();
	while( 1 ) {
		loop();
	}
	return 0;
}
