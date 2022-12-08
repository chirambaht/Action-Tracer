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
	printf( "Initialised\n" );

	if( main_dev->get_connected_clients() == 0 ) {
		printf( "Waiting for client to connect\n" );
		while( main_dev->get_connected_clients() == 0 ) {
			delay( 10 );
		}
	}

	printf( "Connected to client" );

	ActionTracer::Communication::ActionServer *server = main_dev->get_server();
	ActionMessage *							   incoming;

	while( 1 ) {
		try {
			incoming = server->read_packet();
		} catch( std::exception &e ) {
			printf( "Error reading packet: %s", e.what() );
		}

		if( incoming->action() == ActionCommand::MAPPING ) {
			printf( "Mapping command received! Not yet supported" );
		}

		if( incoming->action() == ActionCommand::START ) {
			printf( "Start command received! Starting data collection" );

			main_dev->show_body();
			main_dev->start();
		}

		if( incoming->action() == ActionCommand::STOP ) {
			printf( "Stop command received! Stopping data collection" );
			main_dev->stop();
		}

		if( incoming->action() == ActionCommand::DISCONNECT ) {
			printf( "Disconnect command received! Disconnecting" );
			main_dev->stop();
			server->disconnect_all_clients();
		}

		if( incoming->action() == ActionCommand::UNKNOWN ) {
			printf( "Unknown command. Ignoring" );
		}

		if( incoming->action() == ActionCommand::OUTPUT_RATE ) {
			printf( "Output rate command received! Setting it to %d Hz", incoming->param() );
			main_dev->set_sample_rate( incoming->param() );
		}
	}

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
	while( 1 ) {
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

	printf( "Running basic setup routine\n" );
	setup();

	printf( "\nSetup Complete! Running now\n\n" );
	while( 1 ) {
		loop();
	}

	printf( "For some reason, this has run: main" );
	return 0;
}