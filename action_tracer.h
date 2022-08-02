#ifndef ACTION_TRACER_H
#define ACTION_TRACER_H

#include "tracer_point.h"

#include <vector>
#define MAX_ACT_DEVICES 13

#define ACT_I2C_DATA_WIRING_PI_PIN	8
#define ACT_I2C_CLOCK_WIRING_PI_PIN 9

#define ACT_DEVICE_0_WIRING_PI_PIN	7
#define ACT_DEVICE_1_WIRING_PI_PIN	0
#define ACT_DEVICE_2_WIRING_PI_PIN	2
#define ACT_DEVICE_3_WIRING_PI_PIN	3
#define ACT_DEVICE_4_WIRING_PI_PIN	12
#define ACT_DEVICE_5_WIRING_PI_PIN	13
#define ACT_DEVICE_6_WIRING_PI_PIN	14
#define ACT_DEVICE_7_WIRING_PI_PIN	30
#define ACT_DEVICE_8_WIRING_PI_PIN	21
#define ACT_DEVICE_9_WIRING_PI_PIN	22
#define ACT_DEVICE_10_WIRING_PI_PIN 23
#define ACT_DEVICE_11_WIRING_PI_PIN 24
#define ACT_DEVICE_12_WIRING_PI_PIN 25

#define ACT_DEVICE_0_INT_WIRING_PI_PIN 27

#define ACT_DATA_TEMPERATURE  ( 1 << 0 )
#define ACT_DATA_QUATERNION	  ( 1 << 1 )
#define ACT_DATA_ACCELERATION ( 1 << 2 )
#define ACT_DATA_GYROSCOPE	  ( 1 << 3 )
#define ACT_DATA_EULER		  ( 1 << 4 )
#define ACT_DATA_AIRCRAFT	  ( 1 << 5 )
#define ACT_DATA_GRAVITY	  ( 1 << 6 )

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

#define ACT_BODY_WAIST		   0
#define ACT_BODY_RIGHT_BICEP   ( 1 << 0 )
#define ACT_BODY_RIGHT_FOREARM ( 1 << 1 )
#define ACT_BODY_RIGHT_HAND	   ( 1 << 2 )
#define ACT_BODY_LEFT_BICEP	   ( 1 << 3 )
#define ACT_BODY_LEFT_FOREARM  ( 1 << 4 )
#define ACT_BODY_LEFT_HAND	   ( 1 << 5 )
#define ACT_BODY_CHEST		   ( 1 << 6 )
#define ACT_BODY_HEAD		   ( 1 << 7 )
#define ACT_BODY_RIGHT_THIGH   ( 1 << 8 )
#define ACT_BODY_RIGHT_KNEE	   ( 1 << 9 )
#define ACT_BODY_RIGHT_FOOT	   ( 1 << 10 )
#define ACT_BODY_LEFT_THIGH	   ( 1 << 11 )
#define ACT_BODY_LEFT_KNEE	   ( 1 << 12 )
#define ACT_BODY_LEFT_FOOT	   ( 1 << 13 )
#define ACT_BODY_RIGHT_HIP	   ( 1 << 14 )
#define ACT_BODY_LEFT_HIP	   ( 1 << 15 )

namespace ActionTracer {
	class ActionTracer {
	  private:
		TracePoint			   *_devices_in_use[MAX_ACT_DEVICES] = { new TracePoint() };
		std::vector<TracePoint *> _devices_waiting_for_use;
		uint16_t				  _act_sample_rate;

		void	 show_body();
		uint16_t _get_body_identifier( uint16_t );
		uint16_t _get_ACT_device_pin( uint16_t );

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