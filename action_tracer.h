#ifndef ACTION_TRACER_H
#define ACTION_TRACER_H

#include "act_definitions.h"
#include "packager.h"
#include "tracer_point.h"

#include <thread>
#include <vector>

namespace ActionTracer {

	class ActionTracer {
	  private:
		TracePoint				   *_devices_in_use[MAX_ACT_DEVICES];
		Communication::Supervisor *_supervisor;

		std::vector<TracePoint *> _devices_waiting_for_use;
		uint8_t					  _act_sample_rate = 4;
		float					*_data_package[DATA_PACKAGE_SIZE];
		ActionDataPackage		  *_data_package_action[MAX_ACT_DEVICES] = { new ActionDataPackage() };

		bool _running	 = false;
		bool _paused	 = true;
		bool _data_ready = false;

		uint16_t _get_body_identifier( uint16_t );
		uint8_t	 _get_ACT_device_pin( uint16_t );

		bool _turn_off_all_devices();

		bool _validate_mapping( uint16_t, uint16_t );

		void _data_transmission_thread( Communication::Supervisor *, bool *, bool * );
		void _client_manager_thread( Communication::Supervisor *, bool *, bool * );
		void _data_collection_thread( Communication::Supervisor *, bool *, bool *, bool * );

		std::thread _thread_data_transmission, _thread_client_manager, _thread_data_collection;
		bool		_thread_running_data_transmission = false, _thread_running_client_manager = false, _thread_running_data_collection = false;

	  public:
		ActionTracer( /* args */ );
		~ActionTracer();

		// add copy, move, and assignment constructors
		// ActionTracer( const ActionTracer &other );
		// ActionTracer( ActionTracer &&other );
		// ActionTracer &operator=( const ActionTracer &other );
		// ActionTracer &operator=( ActionTracer &&other );

		void start();
		void stop();
		void pause();
		void resume();
		void reset();

		// void status();

		void initialize();
		void initialize( int8_t );
		// void close();

		// void map_device( uint16_t );
		void map_device( uint16_t, uint16_t );
		void unmap_device();
		void device_status();

		// void set_server_details();

		// void hello();

		// void set_packet_data();

		void	set_fifo_rate( uint8_t );
		void	set_fifo_rate( uint8_t, uint8_t );
		uint8_t get_fifo_rate( uint8_t ) const;
		void	set_sample_rate( uint8_t );
		uint8_t get_sample_rate() const;

		void show_body();

		uint8_t						 get_connected_clients() const;
		Communication::ActionServer *get_server();
	};
} // namespace ActionTracer

#endif // ACTION_TRACER_H