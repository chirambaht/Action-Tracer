// This will be the interactive shell to get the different segments of code working.

/*
The goals of this section is to write code that can:
1. Take note of all the devices that are linked through I2C scan
2. Callibrate all the devices and load their configurations instantly
3. Take readings, consistantly from all devices
4. Save config or reading data as requested.


Remember, on is when we're talking to device 0x69
*/

#include "action.h"

#include "action_pi.hpp"
#include "tracer_point.h"

#include <stddef.h>

#ifdef ON_PI
	#include <wiringPi.h>
#endif

int main( int argc, char const *argv[] ) {
	/* code */
	return 0;
}

void ActionTracer::silence_tracers() {
	for( size_t i = 0; i < ActionTracer::num_action_devices; i++ ) {
		silence_tracer( ActionTracer::get_pi_location( i ) );
	}
}

void ActionTracer::silence_tracer( int pin ) {
#ifdef ON_PI
	digitalWrite( pin, 1 );
#endif
}

int ActionTracer::scan_i2c_for_tracers() {
	// Get all the devices down to no noise
	silence_tracers();
	int res;

	// Store results of each check.
	int alive = 0;

	int life_map[ActionTracer::num_action_devices] = { false };

	for( size_t i = 0; i < ActionTracer::num_action_devices; i++ ) {
#ifdef ON_PI
		life_map[i] = wiringPiI2CSetup( int devId );
#endif
		if( life_map[i] != -1 ) {
			alive++;
		}
	}
	return alive;
}