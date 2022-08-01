#ifndef ACTION_TRACER_H
#define ACTION_TRACER_H

#include "tracer_point.h"

#include <vector>

#define ACT_0  0
#define ACT_1  ( 1 << 0 )
#define ACT_2  ( 1 << 1 )
#define ACT_3  ( 1 << 2 )
#define ACT_4  ( 1 << 3 )
#define ACT_5  ( 1 << 4 )
#define ACT_6  ( 1 << 5 )
#define ACT_7  ( 1 << 6 )
#define ACT_8  ( 1 << 7 )
#define ACT_9  ( 1 << 8 )
#define ACT_10 ( 1 << 9 )
#define ACT_11 ( 1 << 10 )
#define ACT_12 ( 1 << 11 )
#define ACT_13 ( 1 << 12 )

namespace ActionTracer {
	class ActionTracer {
	  private:
		std::vector<TracePoint> _devices_in_use;
		uint16_t				_act_sample_rate;

	  public:
		ActionTracer( /* args */ );
		~ActionTracer();

		// add copy, move, and assignment constructors
		ActionTracer( const ActionTracer &other );
		ActionTracer( ActionTracer &&other );
		ActionTracer &operator=( const ActionTracer &other );
		ActionTracer &operator=( ActionTracer &&other );

		void start();
		void stop();
		void pause();
		void resume();
		void reset();

		void status();

		void initialize( int16_t );
		void close();

		void map_device();
		void unmap_device();
		void device_status();

		void set_server_details();

		void set_packet_data();

		void	set_fifo_rate( uint16_t, uint8_t );
		uint8_t get_fifo_rate( uint16_t ) const;
		void	set_sample_rate( uint8_t );
		uint8_t get_sample_rate() const;
	};

} // namespace ActionTracer

#endif // ACTION_TRACER_H