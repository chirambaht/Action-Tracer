#ifndef _MAIN_H_
#define _MAIN_H_

#include "action_tracer.h"

#include <cstdint>

ActionTracer::ActionTracer *main_dev = new ActionTracer::ActionTracer();

void setup();
void loop();
void exit_handler( int s );

// namespace ActionTracer

#endif