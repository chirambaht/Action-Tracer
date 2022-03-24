#ifndef ACTION_H
#define ACTION_H

namespace ActionTracer {
	int scan_i2c_for_tracers();

	void silence_tracers();
	void silence_tracer( int pin );
} // namespace ActionTracer

#endif