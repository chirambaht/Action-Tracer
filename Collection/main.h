#ifndef _MAIN_H_
#define _MAIN_H_

#define PORT		9022 // Main UDP data port for the data to be sent through
#define MAX_SENSORS 20	 // Number of points track on the body.
#define WAIT_TIME	7	 // Time to before getting next data sample (in ms)

#include "packager.h"
#include "tracer_point.h"

#include <cstdint>
#include <string>

using namespace ActionTracer;

Packager *communicator; // The speaker dude

TracePoint *body_sensor[MAX_SENSORS]; // These are the N sensors on the body.
// std::array<TracePoint, MAX_SENSORS> *body_sensor; // These are the N sensors on the body.

size_t		_sensors		= 0;		   // Number of sensors being used
std::string _address		= "127.0.0.1"; // Address to send the data to for the communicator
float		data_package[4] = { 0 };	   //

bool send_ready = false;

void turn_off_all_devices();

uint8_t data_buffer[512];

void setup();
void loop();
void exit_handler( int s );

long loop_start = 0;
// namespace ActionTracer

#endif