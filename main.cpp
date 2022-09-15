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
#else
	#include "wiringPi.h"
#endif

using namespace ActionTracer;

#ifdef TAKE_ARGUMENTS
cxxopts::Options options( "Action Tracer", "This program runs a given number of MPU6050 IMU's and sends the data packets via UDP." );
#endif

/**
 * @brief Initialise all the devices in the network. Store them in objects in main.h
 * @return 0 if success
 */
void setup() {
	wiringPiSetup();

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = exit_handler;
	sigemptyset( &sigIntHandler.sa_mask );
	sigIntHandler.sa_flags = 0;

	sigaction( SIGINT, &sigIntHandler, NULL );
	main_dev->show_body();
	// main_dev->map_device( ACT_0, ACT_BODY_WAIST );
	main_dev->map_device( ACT_1, ACT_BODY_LEFT_BICEP );
	main_dev->map_device( ACT_2, ACT_BODY_LEFT_FOREARM );
	main_dev->map_device( ACT_3, ACT_BODY_LEFT_HAND );
	main_dev->show_body();
	main_dev->set_sample_rate( 2 );
	printf( "All set to go \n" );
	main_dev->initialize();
	main_dev->show_body();
	printf( "Initialised\n" );
	main_dev->start();
	printf( "Started and will run for 10 seconds\n" );
	// wait for 10 seconds
	// delay( 10000 );

	main_dev->stop();
	printf( "Stopped and done!\n" );
	exit( 1 );
}

void exit_handler( int s ) {
	printf( "\nTurning off all devices...\n" );
	main_dev->stop();
	exit( 1 );
}

/**
 * @brief Main work loop for all the code. This will always run every cycle.
 * @return Nothing
 */
void loop() {
	while ( 1 ) {
		/* code */
	}
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
#else

#endif

	// TODO: Run a new setup method that accounts for debug, custom tps, files and
	printf( "Running basic setup routine\n" );
	setup();

	printf( "\nSetup Complete! Running now\n\n" );
	while ( 1 ) {
		loop();
	}

	printf( "For some reason, this has run: main" );
	return 0;
}