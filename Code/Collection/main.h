#ifndef _MAIN_H_
#define _MAIN_H_

// #define ARRAY_SOLUTION

// #define COUNT_FRAMES

#define PORT		9022 // Main UDP data port for the data to be sent through
#define MAX_SENSORS 20	 // Number of points track on the body.

#include "action_pi.hpp"
#include "packager.h"
#include "tracer_point.h"

#include <array>
#include <memory.h>
#include <vector>

using namespace ActionTracer;

Packager *communicator; // The speaker dude

TracePoint *body_sensor[MAX_SENSORS]; // These are the N sensors on the body.
// std::array<TracePoint, MAX_SENSORS> *body_sensor; // These are the N sensors on the body.

size_t		_sensors		= 0;		   // Number of sensors being used
std::string _address		= "127.0.0.1"; // Address to send the data to for the communicator
float		data_package[4] = { 0 };	   //

// Heart rate sensor variables
uint32_t	  aun_ir_buffer[100];  // infrared LED sensor data
uint32_t	  aun_red_buffer[100]; // red LED sensor data
int32_t		  n_ir_buffer_length;  // data length
int32_t		  n_spo2;			   // SPO2 value
int8_t		  ch_spo2_valid;	   // indicator to show if the SPO2 calculation is valid
int32_t		  n_heart_rate;		   // heart rate value
int8_t		  ch_hr_valid;		   // indicator to show if the heart rate calculation is valid
uint8_t		  uch_dummy;
uint32_t	  un_min				  = 0x3FFFF;
uint32_t	  un_max				  = 0;
uint32_t	  un_prev_data			  = 0;
const uint8_t hr_ir_red_buffer_length = 100;

uint8_t data_buffer[512];

bool ready_for_hr = false;
bool i2c_busy	  = false;

uint32_t collected_hr_samples = 0;

void setup();
void loop();
void exit_handler( int s );
void prepare_heart_rate_sensor();
void read_heart_rate_fifo();
// namespace ActionTracer

#endif