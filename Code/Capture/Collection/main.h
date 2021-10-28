#ifndef _MAIN_H_
#define _MAIN_H_

#define SEND_TO_MOBILE

#define SERVER_IP  "192.168.1.102" // Server IP where data will be sent to
#define MOBILE_IP  "127.0.0.1"	   // Mobile IP where data will be sent to
#define PORT	   9022			   // Main UDP data port for the data to be sent through
#define N		   20;			   // Number of points track on the body.
#define LOOP_DELAY 0			   // ms to wait per loop cycle

#include "packager.h"
#include "tracer_point.h"

using namespace ActionTracer;

Packager *	communicator;	// The speaker dude
TracePoint *body_sensor[N]; // These are the N sensors on the body.
int			_sensors = 0;	// Sensors being used
bool		_debug	 = false;

// Packager mobile = new Packager( MOBILE_IP, PORT );

void setup( int );
void loop();
// namespace ActionTracer

#endif