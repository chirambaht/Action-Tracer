#ifndef _TRACER_POINT_H_
#define _TRACER_POINT_H_

#include "MPU6050.h"
#include "act_definitions.h"

#include <cstdint>
#include <string>

#define MAXIMUM_COLLECTED_DATA_POINTS ( 20 )

#define ACCELEROMETER_DATA_POINT_SIZE	( 3 )
#define GYROSCOPE_DATA_POINT_SIZE		( 3 )
#define MAGNETOMETER_DATA_POINT_SIZE	( 3 )
#define QUATERNION_DATA_POINT_SIZE		( 4 )
#define EULER_DATA_POINT_SIZE			( 3 )
#define ROTATION_MATRIX_DATA_POINT_SIZE ( 9 )
#define YAW_PITCH_ROLL_DATA_POINT_SIZE	( 3 )
#define TEMPERATURE_DATA_POINT_SIZE		( 1 )
#define GRAVITY_DATA_POINT_SIZE			( 3 )

#define MPU6050_FIFO_BUFFER_SIZE 64

namespace ActionTracer {
	class TracePoint {
	  private:
		MPU6050 *_device;
		uint16_t _identifier = 0;
		uint8_t	 _pin_number = 0;

		uint8_t _device_status = 0;
		int16_t _offsets[6]	   = { 0 }; // ACC x y z GYRO x y z

		bool _dmp_ready				= false;
		bool _device_interrupt_flag = false;
		bool _calibrate				= true;
		bool _device_initialized	= false;

		uint8_t _calibration_samples = 250;

		uint8_t _device_interrupt_status;

		uint8_t	 _fifo_buffer[MPU6050_FIFO_BUFFER_SIZE] = { 0 };
		uint8_t	 _packet_size							= 0;
		uint16_t _fifo_count							= 0;
		uint8_t	 _fifo_rate								= 3;

		Quaternion	_quaternion_packet;
		VectorInt16 _acceleration_packet;
		VectorInt16 _gyroscope_packet;
		float		_temperature_packet = 0;

		float _acceleration_float_packet[ACCELEROMETER_DATA_POINT_SIZE] = { 0 };
		float _gyroscope_float_packet[GYROSCOPE_DATA_POINT_SIZE]		= { 0 };
		float _quaternion_float_packet[QUATERNION_DATA_POINT_SIZE]		= { 0 };

		ActionDataPackage _data_package;

		void _deselect_me();
		void _initialize();
		void _select_me();
		void _set_device_offsets();

	  public:
		// I dont know what this is but I love you
		TracePoint( uint16_t identifier, uint8_t wiring_Pi_pin_number );
		TracePoint();

		~TracePoint();

		void dump_variables();
		void get_data();
		void get_data( bool );
		void set_calibrate( bool, uint8_t );
		void set_calibrate( bool );
		void turn_off();

		void	set_sample_rate( uint8_t );
		uint8_t get_sample_rate() const;

		void			   initialize( uint8_t, uint8_t );
		ActionDataPackage *read_data_action( bool );
		MPU6050			   get_device();
		uint8_t			   get_data_packet_size();
		uint8_t			   identify();

		uint16_t	get_pin_number() const;
		std::string get_act_pin_number_as_string() const;
		void		set_pin_number( uint8_t );
		uint32_t	get_identifier() const;
		void		set_identifier( uint32_t );

		bool is_active();
	};
} // namespace ActionTracer

#endif
