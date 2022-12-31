#ifndef _MAIN_H_
#define _MAIN_H_

#include "action_tracer.h"

#include <cstdint>

ActionTracer::ActionTracer *main_dev	= new ActionTracer::ActionTracer();
ActionTracer::TracePoint *	interrupter = new ActionTracer::TracePoint( ACT_0, ACT_BODY_WAIST );

bool *running;

void setup();
void loop();
void exit_handler( int s );

void swapper();

// namespace ActionTracer

#endif