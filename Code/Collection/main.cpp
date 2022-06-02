// This is the main file that will be used to run the program for data
// collection from the 3 IMU's and send them to the server as is necessary.

#include "main.h"

#include "debug_printer.h"

#ifdef TAKE_ARGUMENTS
	#include <cxxopts.hpp>
#endif

#include <signal.h>

#ifdef ON_PI
	#include <wiringPi.h>
#endif

using namespace ActionTracer;

#ifdef TAKE_ARGUMENTS
cxxopts::Options options( "Action Tracer", "This program runs a given number of MPU6050 IMU's and sends the data packets via UDP." );
#endif

#ifdef ON_PI
PI_THREAD( net_worker ) {
	communicator					 = new Packager( _address, PORT ); // Initialize the communicator that will send data packets to the server
	communicator->_number_of_devices = _sensors;
	communicator->save_enable( true );
	communicator->init_tcp();

	for( ;; ) {
		while( !send_ready ) {
			continue;
		}
		// Send packet
		communicator->send_packet();

		piLock( 1 );
		send_ready = false;
		piUnlock( 1 );
	}
}
#endif

void turn_off_all_devices() {
	for( size_t i = 0; i < _sensors; i++ ) {
		body_sensor[i]->turn_off();
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
	if( piThreadCreate( net_worker ) != 0 ) {
		debugPrint( "Failed to create network worker thread\n" );
		exit( EXIT_FAILURE );
	}
#endif
	std::string name;
	for( size_t i = 0; i < _sensors; i++ ) {
		if( i == 0 ) {
			name = "IMU_1";
		} else if( i == 1 ) {
			name = "IMU_2";
		} else if( i == 2 ) {
			name = "IMU_3";
		}
		body_sensor[i] = new TracePoint( name, get_pi_location( i + 1 ) ); // offset by 1 is to ensure we are starting at ACT_DEVICE_1
	}
}

void exit_handler( int s ) {
	debugPrint( "\n" );
	turn_off_all_devices();
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
	float *body;

	for( size_t i = 0; i < _sensors; i++ ) {
		body = body_sensor[i]->read_data( 1 );
		for( size_t j = 0; j < 4; j++ ) {
			data_package[j] = *body;
			body++;
		}
		communicator->load_packet( data_package, 4 );
	}
#ifdef ON_PI
	piLock( 1 );
	send_ready = true;
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
	options.add_options()( "a,address", "Address to send UDP packets to", cxxopts::value<std::string>()->default_value( "127.0.0.1" ) );
	options.add_options()( "f,file", "Define variables using a file. If a file is given, all other given parameters will be overwritten.", cxxopts::value<std::string>()->default_value( "" ) );
	options.add_options()( "h,help", "Print usage" );
	options.add_options()( "t,tracepoints", "Number of body devices being used on the body", cxxopts::value<int>()->default_value( "0" ) );

	auto result = options.parse( argc, argv );

	if( result.count( "help" ) ) {
		printf( "%s", options.help().c_str() );
		exit( 0 );
	}

	_address = result["address"].as<std::string>();

	_sensors = result["tracepoints"].as<int>();

	if( result.count( "file" ) ) {
		// TODO: Read CSV file for parameters for each item
	}
#else

	#ifndef SEND_ADDRESS
	// Read from setup.csv
	std::ifstream infile( "setup.csv" );
	if( !infile.good() ) {
		_address = "";
	} else {
		// If file exists, use it to set offsets.
		std::string	  line;
		std::ifstream myfile( "setup.csv" );
		std::getline( myfile, line );

		std::stringstream point_line( line );
		std::string		  value;
		size_t			  count = 0;

		while( getline( point_line, value, ',' ) ) {
			// First check if the needed pin exists. If not, set defaults
			if( count == 0 ) {
				_address = value;
			}
		}
	#else
	_address = SEND_ADDRESS;
	#endif
		_sensors = 3;
		printf( "Devices connected: %d\n", _sensors );
#endif

	// TODO: Run a new setup method that accounts for debug, custom tps, files and addresses
	setup();

	while( 1 ) {
		loop();
	}
	return 0;
}