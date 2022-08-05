#ifndef _TRACER_POINT_H_
#define _TRACER_POINT_H_

#include "MPU6050.h"

#include <cstdint>

#define MAXIMUM_COLLECTED_DATA_POINTS ( 20 + 1 )

#define ACCELEROMETER_DATA_POINT_SIZE	( 3 + 1 ) // +1 for identifier
#define GYROSCOPE_DATA_POINT_SIZE		( 3 + 1 ) // +1 for identifier
#define MAGNETOMETER_DATA_POINT_SIZE	( 3 + 1 ) // +1 for identifier
#define QUATERNION_DATA_POINT_SIZE		( 4 + 1 ) // +1 for identifier
#define EULER_DATA_POINT_SIZE			( 3 + 1 ) // +1 for identifier
#define ROTATION_MATRIX_DATA_POINT_SIZE ( 9 + 1 ) // +1 for identifier
#define YAW_PITCH_ROLL_DATA_POINT_SIZE	( 3 + 1 ) // +1 for identifier
#define TEMPERATURE_DATA_POINT_SIZE		( 1 + 1 ) // +1 for identifier
#define GRAVITY_DATA_POINT_SIZE			( 3 + 1 ) // +1 for identifier

#define MPU6050_FIFO_BUFFER_SIZE 64

namespace ActionTracer {
	class TracePoint {
	  private:
		MPU6050 *_device;
		uint16_t _identifier = 0;
		uint8_t	 _pin_number = 0;

		uint8_t _device_status;
		int16_t _offsets[6] = { 0 }; // ACC x y z GYRO x y z

		bool _dmp_ready;
		bool _device_interrupt_flag;
		bool _calibrate = true;

		uint8_t _device_interrupt_status;

		uint8_t	 _fifo_buffer[MPU6050_FIFO_BUFFER_SIZE] = { 0 };
		uint8_t	 _packet_size;
		uint16_t _fifo_count;
		uint8_t	 _fifo_rate = 3;

		Quaternion	_quaternion_packet;
		VectorInt16 _acceleration_packet;
		VectorInt16 _gyroscope_packet;
		VectorFloat _gravity_packet;

		float _complete_float_packet[MAXIMUM_COLLECTED_DATA_POINTS] = { 0 };

		float _acceleration_float_packet[ACCELEROMETER_DATA_POINT_SIZE] = { 0 };
		float _euler_packet[EULER_DATA_POINT_SIZE]						= { 0 };
		float _gravity_float_packet[GRAVITY_DATA_POINT_SIZE]			= { 0 };
		float _gyroscope_float_packet[GYROSCOPE_DATA_POINT_SIZE]		= { 0 };
		float _quaternion_float_packet[QUATERNION_DATA_POINT_SIZE]		= { 0 };
		float _temperature_packet[TEMPERATURE_DATA_POINT_SIZE]			= { 0 };
		float _yaw_pitch_roll_packet[YAW_PITCH_ROLL_DATA_POINT_SIZE]	= { 0 };

		bool _device_initialized = false;

		void _deselect_me();
		void _initialize();
		void _select_me();
		void _set_device_offsets();
		void _set_default_device_offsets();

	  public:
		// I dont know what this is but I love you
		TracePoint( uint8_t identifier, uint8_t wiring_Pi_pin_number );
		TracePoint();

		~TracePoint();

		void dump_variables();
		void get_data();
		void print_last_data_packet();
		void set_calibrate( bool );
		void tracepoint_isr();
		void turn_off();

		void	set_sample_rate( uint8_t );
		uint8_t get_sample_rate() const;

		void	initialize( uint8_t, uint8_t );
		bool	calibrate( uint8_t );
		float  *read_data( int get_data );
		MPU6050 get_device();
		uint8_t get_data_packet_size();
		uint8_t identify();

		uint16_t get_pin_number();
		void	 set_pin_number( uint16_t );
		uint16_t get_identifier();
		void	 set_identifier( uint16_t );

		bool is_active();
	};
} // namespace ActionTracer

#endif
