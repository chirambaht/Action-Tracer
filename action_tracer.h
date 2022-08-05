#ifndef ACTION_TRACER_H
#define ACTION_TRACER_H

#include "act_definitions.h"
#include "packager.h"
#include "tracer_point.h"

#include <vector>

namespace ActionTracer {
	class ActionTracer {
	  private:
		TracePoint *_devices_in_use[MAX_ACT_DEVICES] = { new TracePoint() };
		Packager	 *_communicator;

		std::vector<TracePoint *> _devices_waiting_for_use;
		uint16_t				  _act_sample_rate;
		float					*_data_package[DATA_PACKAGE_SIZE];

		void *data_collection_thread();
		void *data_sending_thread();

		bool _running = false;
		bool _paused  = false;

		void	 show_body();
		uint16_t _get_body_identifier( uint16_t );
		uint16_t _get_ACT_device_pin( uint16_t );

		bool _validate_mapping( uint16_t, uint16_t );

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

		void initialize( int8_t );
		void close();

		void map_device( uint16_t );
		void map_device( uint16_t, uint16_t );
		void unmap_device();
		void device_status();

		void set_server_details();

		void set_packet_data();

		void	set_fifo_rate( uint8_t, uint8_t );
		uint8_t get_fifo_rate( uint8_t ) const;
		void	set_sample_rate( uint8_t );
		uint8_t get_sample_rate() const;
	};

} // namespace ActionTracer

#endif // ACTION_TRACER_H