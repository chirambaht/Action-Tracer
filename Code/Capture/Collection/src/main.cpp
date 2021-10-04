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
	MPU6050 *d0 = new MPU6050( MPU6050_ADDRESS_AD0_HIGH );
	MPU6050 *d1 = new MPU6050( MPU6050_ADDRESS_AD0_HIGH );
	MPU6050 *d2 = new MPU6050( MPU6050_ADDRESS_AD0_HIGH );

	body_sensor[0] = new TracePoint( d0, "Left Shoulder", 2, 1 );
	body_sensor[1] = new TracePoint( d1, "Left Forearm", 2, 1 );
	body_sensor[2] = new TracePoint( d2, "Left Hand", 2, 1 );

	debugPrint( "Looking at sensor 0, Left Shoulder" );
	body_sensor[0].identify();
}

/*
  Main work loop for all the code. This will always run every cycle.
*/
void loop() {
	float data_package[N * 4];
	for( size_t i = 0; i < N; i++ ) {
		data_package[i * 4] = body_sensor[i].read_data( 0 );
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
