// This is the main file that will be used to run the program for data
// collection from the 3 IMU's and send them to the server as is necessary.

#include "main.h"

#include "debug_printer.h"
#include "hr_algorithm.h"

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
// #include <sys/time.h>
// #include <sys/timeb.h>
// #include <thread>
#include <unistd.h>

#ifdef ON_PI
	#include <wiringPi.h>
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
#ifdef ON_PI
	wiringPiSetup();
#endif

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = exit_handler;
	sigemptyset( &sigIntHandler.sa_mask );
	sigIntHandler.sa_flags = 0;

	sigaction( SIGINT, &sigIntHandler, NULL );

	communicator					 = new Packager( _address, PORT ); // Initialize the communicator that will send data packets to the server
	communicator->_number_of_devices = _sensors;
	communicator->save_enable( true );
	communicator->init_tcp(); // Will break here if it doesn't find a TCP socket available

	for( size_t i = 0; i < _sensors; i++ ) {
		body_sensor[i] = new TracePoint( "", get_pi_location( i + 1 ) );
		// body_sensor[i] = new TracePoint( "", get_pi_location( i ) );
	}

	maxim_max30102_reset();
	maxim_max30102_read_reg( REG_INTR_STATUS_1, &uch_dummy ); // Reads/clears the interrupt status register
	maxim_max30102_init();									  // initialize the MAX30102

#ifdef ON_PI
	// Adds an interrupt to across ACT 4 for the heart rate sensor to read the FIFO
	int resp = wiringPiISR( ACT_DEVICE_4, INT_EDGE_BOTH, read_heart_rate_fifo );
	if( resp < 0 ) {
		debugPrint( "Error. Got code %d\n", resp ); // ISR failed to init.
	}
#endif

	debugPrint( "Waiting for HR pre-samples" );
	while( collected_hr_samples < 100 ) {
		if( collected_hr_samples == 50 ) {
			debugPrint( ".... " );
		} else if( collected_hr_samples == 75 ) {
			debugPrint( ".... " );
		} else if( collected_hr_samples == 99 ) {
			debugPrint( "...." );
		}
		continue;
	}
	debugPrint( "\nDone collecting first samples.\n" );
}

void exit_handler( int s ) {
	debugPrint( "\n" );
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
		i2c_busy = true;
		body	 = body_sensor[i]->read_data( 1 );
		i2c_busy = false;

		for( size_t j = 0; j < 4; j++ ) {
			data_package[j] = *body;
			body++;
		}
		communicator->load_packet( data_package, 4 );
	}

	if( i2c_hr_waiting ) {
		read_heart_rate_fifo();
		i2c_hr_waiting = false;
	}

	data_package[0] = n_heart_rate;
	data_package[1] = n_spo2;
	data_package[2] = ch_hr_valid;
	data_package[3] = ch_spo2_valid;
	communicator->load_packet( data_package, 4 );

	// Send packet
	communicator->send_packet();
}

void read_heart_rate_fifo() {
	while( i2c_busy ) {
		i2c_hr_waiting = true;
		return;
	}

	maxim_max30102_read_fifo( ( aun_red_buffer + collected_hr_samples ), ( aun_ir_buffer + collected_hr_samples ) ); // read from MAX30102 FIFO
	maxim_max30102_read_reg( REG_INTR_STATUS_1, &uch_dummy );														 // Reads/clears the interrupt status register

	if( i2c_hr_waiting ) {
		i2c_hr_waiting = false;
	}

	if( un_min > aun_red_buffer[collected_hr_samples] )
		un_min = aun_red_buffer[collected_hr_samples]; // update signal min
	if( un_max < aun_red_buffer[collected_hr_samples] )
		un_max = aun_red_buffer[collected_hr_samples]; // update signal max

	if( collected_hr_samples < 100 ) {
		collected_hr_samples++;
		un_prev_data = aun_red_buffer[collected_hr_samples];
		return;
	}

	// shift 75 samples back in the buffer to get rid of 25
	for( int i = 25; i < 100; i++ ) {
		aun_red_buffer[i - 25] = aun_red_buffer[i];
		aun_ir_buffer[i - 25]  = aun_ir_buffer[i];

		// update the signal min and max
		if( un_min > aun_red_buffer[i] )
			un_min = aun_red_buffer[i];
		if( un_max < aun_red_buffer[i] )
			un_max = aun_red_buffer[i];
	}

	// take 25 sets of samples before calculating the heart rate.
	int current_ir_red_count = collected_hr_samples;
	for( int i = 75; i < 100; i++ ) {
		un_prev_data = aun_red_buffer[i - 1];

		while( collected_hr_samples - current_ir_red_count < 25 ) {
			continue; // Wait for 25 values to be collected
		}

		if( !( ch_hr_valid && ch_spo2_valid ) || ( n_heart_rate > 180 || n_heart_rate < 30 ) ) {
			continue;
		}
	}

	maxim_heart_rate_and_oxygen_saturation( aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid );
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