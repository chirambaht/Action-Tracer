#ifndef _MAIN_H_
#define _MAIN_H_

#define SEND_TO_MOBILE

#define ARRAY_SOLUTION
// #define VECTOR_SOLUTION
// #define INTERRUPT_SOLUTION

#define COUNT_FRAMES

#define PORT		9022 // Main UDP data port for the data to be sent through
#define MAX_SENSORS 20	 // Number of points track on the body.
#define LOOP_DELAY	0	 // ms to wait per loop cycle

#include "packager.h"
#include "tracer_point.h"

#include <array>
#include <memory.h>
#include <vector>

using namespace ActionTracer;

#ifdef ARRAY_SOLUTION
Packager *	communicator;			  // The speaker dude
TracePoint *body_sensor[MAX_SENSORS]; // These are the N sensors on the body.
// std::array<TracePoint, MAX_SENSORS> *body_sensor; // These are the N sensors on the body.
uint8_t PI_ORDER[6] = { 0, 2, 3, 12, 13, 14 };
#endif

#ifdef INTERRUPT_SOLUTION
Packager *	communicator;			  // The speaker dude
TracePoint *body_sensor[MAX_SENSORS]; // These are the N sensors on the body.
uint8_t		PI_ORDER[6] = { 0, 2, 3, 12, 13, 14 };
#endif

#ifdef POINTER_SOLUTION
auto					  body_sensor = std::make_unique<TracePoint[]>( MAX_SENSORS );
std::unique_ptr<Packager> communicator;
#endif

#ifdef VECTOR_SOLUTION

std::vector<TracePoint> bodysensors;
#endif
size_t		_sensors = 0; // Sensors being used
bool		_debug	 = true;
std::string _address = "127.0.0.1";
float		data_package[4];

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
// Packager mobile = new Packager( MOBILE_IP, PORT );

void setup( int );
void loop();
void basic_isr();
// namespace ActionTracer

#endif