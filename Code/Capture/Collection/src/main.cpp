// This is the main file that will be used to run the program for data
// collection from the 3 IMU's and send them to the server as is necesarry.
#include "main.h"
/// Outline

using namespace ActionTracer;

/*
    Initialise all the devices in the network. Store them in objects in main.h
*/

void setup() {
	communicator = new Packager( SERVER_IP, PORT ); // Initialize the communicator that will send data packets to the server

	// The following are the sensors being used to obtain data from the body.
	body_sensor[0] = new TracePoint( new MPU6050( MPU6050_ADDRESS_AD0_HIGH ), "Left Shoulder", 2, 1 );
	body_sensor[1] = new TracePoint( new MPU6050( MPU6050_ADDRESS_AD0_HIGH ), "Left Forearm", 2, 1 );
	body_sensor[2] = new TracePoint( new MPU6050( MPU6050_ADDRESS_AD0_HIGH ), "Left Hand", 2, 1 );

	debugPrint( "Looking at sensor 0, Left Shoulder" );
	body_sensor[0].identify();
}

/*
  Main work loop for all the code. This will always run every cycle.
*/
void loop() {
	float data_package[N * 4];
	for( size_t i = 0; i < N; i++ ) {
		data_package[N * 4] = body_sensor[i].read_data();
	}
	communicator.send_packet( data_package, N * 4 );
	usleep( LOOP_DELAY * 1000 );
}

int main( int argc, char const *argv[] ) {
	setup();
	while( 1 ) {
		loop();
	}
	return 0;
}
