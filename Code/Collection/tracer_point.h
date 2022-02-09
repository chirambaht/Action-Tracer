#ifndef _TRACER_POINT_H_
#define _TRACER_POINT_H_

// #include "MPU6050_6Axis_MotionApps20.h"
#include "MPU6050.h"

#include <string>

namespace ActionTracer {
	class TracePoint {
	  private:
		bool		_debug = false;
		MPU6050 *	_device;
		std::string _device_name;
		int			_pin_number;

		uint8_t _device_status;

		bool _dmp_ready;

		uint8_t _device_interrupt_status;
		bool	_device_interrupt_flag;

		uint8_t	 _fifo_buffer[64];
		uint16_t _packet_size;
		uint16_t _fifo_count;

		Quaternion _quaternion_packet;

		VectorInt16 _acceleration_packet;
		VectorInt16 _gyroscope_packet;
		VectorFloat _gravity_packet;
		float		_gyroscope_float_packet[3]	  = { 0 };
		float		_acceleration_float_packet[3] = { 0 };
		float		_gravity_float_packet[3]	  = { 0 };
		float		_quaternion_float_packet[4]	  = { 0 };
		float		_euler_packet[3]			  = { 0 };
		float		_yaw_pitch_roll_packet[3]	  = { 0 };

		void _select_me();
		void _deselect_me();
		void _set_device_offsets();
		void _set_default_device_offsets();

	  public:
		// Creates and inits a device
		//I dont know what this is but I love you
		TracePoint( std::string name, int wiring_Pi_pin_number );
		TracePoint( std::string name, int wiring_Pi_pin_number, int interrupt_pin );
		TracePoint();

		~TracePoint();

		// TracePoint operator=( const TracePoint other );
		void		set_debug( bool );
		std::string get_name();
		MPU6050		get_device();
		void		get_data();				   // Gets data from the device it is attached to
		float *		read_data( int get_data ); // Reads the internally stored data and optionally fetches data first
		std::string identify();				   // Blinks the sensor for 5 seconds. Realistically, it is raising the interupt line for the device in quesion
		void		print_last_data_packet();  // Prints the last data packet obtained by this node
		void		tracepoint_isr();
	};
} // namespace ActionTracer

#endif