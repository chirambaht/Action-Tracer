#ifndef _MAIN_H_
#define _MAIN_H_

#define SEND_TO_MOBILE

#define PORT		9022 // Main UDP data port for the data to be sent through
#define MAX_SENSORS 20	 // Number of points track on the body.
#define LOOP_DELAY	0	 // ms to wait per loop cycle

#include "packager.h"
#include "tracer_point.h"

using namespace ActionTracer;

Packager *	communicator;			  // The speaker dude
TracePoint *body_sensor[MAX_SENSORS]; // These are the N sensors on the body.
size_t		_sensors = 0;			  // Sensors being used
bool		_debug	 = false;
std::string _address = "127.0.0.1";

// Packager mobile = new Packager( MOBILE_IP, PORT );

void setup( int );
void loop();
// namespace ActionTracer

#endif