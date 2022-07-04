#ifndef _TRACER_POINT_H_
#define _TRACER_POINT_H_

#include "MPU6050.h"

#include <cstdint>

namespace ActionTracer {
	class TracePoint {
	  private:
		MPU6050 *_device;
		uint8_t	 _identifier;
		int		 _pin_number;

		uint8_t _device_status;

		bool _dmp_ready;

		uint8_t _device_interrupt_status;
		bool	_device_interrupt_flag;

		uint8_t	 _fifo_buffer[64] = { 0 };
		uint16_t _packet_size;
		uint16_t _fifo_count;

		Quaternion _quaternion_packet;

		VectorInt16 _acceleration_packet;
		VectorInt16 _gyroscope_packet;
		VectorFloat _gravity_packet;

		float _gyroscope_float_packet[3]	= { 0 };
		float _acceleration_float_packet[3] = { 0 };
		float _gravity_float_packet[3]		= { 0 };
		float _quaternion_float_packet[4]	= { 0 };
		float _euler_packet[3]				= { 0 };
		float _yaw_pitch_roll_packet[3]		= { 0 };

		__int16_t _teapot_raw_packet[10] = { 0 };

		void _select_me();
		void _deselect_me();
		void _set_device_offsets();
		void _set_default_device_offsets();

	  public:
		// Creates and inits a device
		// I dont know what this is but I love you
		TracePoint( uint8_t identifier, uint8_t wiring_Pi_pin_number );
		TracePoint( uint8_t identifier, uint8_t wiring_Pi_pin_number, uint8_t interrupt_pin );
		TracePoint();

		~TracePoint();

		void	turn_off();
		MPU6050 get_device();
		void	get_data();				   // Gets data from the device it is attached to
		float  *read_data( int get_data ); // Reads the internally stored data and optionally fetches data first
		uint8_t identify();				   // Blinks the sensor for 5 seconds. Realistically, it is raising the interupt line for the device in quesion
		void	print_last_data_packet();  // Prints the last data packet obtained by this node
		void	tracepoint_isr();
	};
} // namespace ActionTracer

#endif