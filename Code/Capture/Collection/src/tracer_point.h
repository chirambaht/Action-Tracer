#ifndef _TRACER_POINT_H_
#define _TRACER_POINT_H_

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "debug_printer.h"

#include <string>
#include <wiringPi.h>

#define OK	  0
#define ERROR 1

#define GET_DATA_QUATERNION	   0
#define GET_DATA_EULER		   1
#define GET_DATA_ALL		   2
#define GET_DATA_GYROSCOPE	   3
#define GET_DATA_ACCELEROMETER 4
#define GET_DATA_YAWPITCHROLL  5

namespace ActionTracer {
	class TracePoint {
	  private:
		MPU6050 *	_device;
		std::string _device_name;
		int			_pin_number;
		uint8_t		_output_data_type;

		uint8_t _dmp_status;
		uint8_t _dmp_ready;

		uint8_t	 _fifo_buffer[64];
		uint16_t _packet_size;
		uint16_t _fifo_count;

		Quaternion _quaternion_packet;

		VectorInt16 _acceleration_packet;
		VectorInt16 _gyroscope_packet;
		VectorFloat _gravity_packet;
		float		_gyroscope_float_packet[3];
		float		_acceleration_float_packet[3];
		float		_gravity_float_packet[3];
		float		_quaternion_float_packet[4];
		float		_euler_packet[3];
		float		_yaw_pitch_roll_packet[3];

		int _select_me();
		int _deselect_me();

	  public:
		// Creates and inits a device
		//I dont know what this is but I love you
		TracePoint( MPU6050 *dev, std::string name, int wiring_Pi_pin_number, int output_data );
		TracePoint();

		~TracePoint();
		std::string get_name();
		MPU6050		get_device();
		void		get_data();						  // Gets data from the device it is attached to
		float *		read_data( int get_data );		  // Reads the internally stored data and optionally fetches data first
		std::string identify();						  // Blinks the sensor for 5 seconds. Realistically, it is raising the interupt line for the device in quesion
		void		print_last_data_packet();		  // Prints the last data packet obtained by this node
		void		set_output_data_type( int type ); // Set which data type to get back in the packets.
	};
} // namespace ActionTracer

#endif