// This is the main file that will be used to run the program for data
// collection from the 3 IMU's and send them to the server as is necessary.

#include "action_pi.hpp"
#include "debug_printer.h"
#include "main.h"

#include <signal.h>
#include <stdlib.h>

#ifdef ON_PI
	#include <wiringPi.h>
#else
	#include "wiringPi.h"
#endif

using namespace ActionTracer;

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

	// Only one device being used for now
	main_dev->map_device( ACT_0, ACT_BODY_WAIST ); // The device onboard the HAT

	printf( "All set to go \n" );

	main_dev->initialize();
	main_dev->show_body();
	printf( "Initialised\n" );

	if ( main_dev->get_connected_clients() == 0 ) {
		printf( "Waiting for client to connect\n" );
		while ( main_dev->get_connected_clients() == 0 ) {
			delay( 10 );
		}
	}

	main_dev->start();
	printf( "Started and will run for 1 minute\n" );

	delay( 60000 );

	main_dev->stop();
	printf( "Stopped\n" );
	exit( 0 );
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
	// TODO: Run a new setup method that accounts for debug, custom tps, files and
	if ( argc < 2 ) {
		printf( "Usage: %s <rate>\n", argv[0] );
		printf( "Output data rate is calculated as: 200/(rate+1)\n\n" );
		printf( " Output Rate  |  Rate\n" );
		printf( "--------------+-------\n" );
		printf( "     200Hz    |  0\n" );
		printf( "     100Hz    |  1\n" );
		printf( "     67Hz     |  2\n" );
		printf( "     50Hz     |  3\n" );
		printf( "     40Hz     |  4\n" );
		printf( "     33Hz     |  5\n" );
		printf( "     20Hz     |  9\n" );
		printf( "     10Hz     |  19\n" );
		printf( "     5Hz      |  39\n" );
		printf( "     1Hz      |  199\n" );

		return 1;
	}

	main_dev->set_sample_rate( atoi( argv[1] ) );

	printf( "Running basic setup routine\n" );
	setup();

	printf( "\nSetup Complete! Running now\n\n" );
	while ( 1 ) {
		loop();
	}

	printf( "For some reason, this has run: main" );
	return 0;
}