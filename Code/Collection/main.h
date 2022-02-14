#ifndef _MAIN_H_
#define _MAIN_H_

// #define ARRAY_SOLUTION
// #define VECTOR_SOLUTION

// #define COUNT_FRAMES

#define PORT		9022 // Main UDP data port for the data to be sent through
#define MAX_SENSORS 20	 // Number of points track on the body.

#include "packager.h"
#include "tracer_point.h"

#include <array>
#include <memory.h>
#include <vector>

using namespace ActionTracer;

Packager *communicator;							 // The speaker dude
uint8_t	  PI_ORDER[6] = { 0, 2, 3, 12, 13, 14 }; // All the Pi pins that can be used.

#ifdef ARRAY_SOLUTION
TracePoint *body_sensor[MAX_SENSORS]; // These are the N sensors on the body.
									  // std::array<TracePoint, MAX_SENSORS> *body_sensor; // These are the N sensors on the body.
#endif

#ifdef VECTOR_SOLUTION
std::vector<TracePoint> bodysensors;
#endif

size_t		_sensors		= 0; // Sensors being used
bool		_debug			= true;
std::string _address		= "127.0.0.1";
float		data_package[4] = { 0 };

#ifdef COUNT_FRAMES
uint32_t _start_time				   = 0;
uint64_t _packets_sent				   = 0;
uint64_t _packets_collected			   = 0;
uint32_t _packets_sent_per_second	   = 0;
uint32_t _packets_collected_per_second = 0;
float	 _average_packets_sent		   = 0;
float	 _average_packets_collected	   = 0;
uint32_t _seconds_since_start		   = 0;
bool	 first_second				   = false;
#endif

void setup( int );
void loop();
void exit_handler( int s )

// namespace ActionTracer

#endif