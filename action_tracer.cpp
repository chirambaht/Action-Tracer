#include "action_tracer.h"

#include <cstdio>
#include <stdexcept>

const uint8_t PI_ORDER[13] = { ACT_DEVICE_0_WIRING_PI_PIN, ACT_DEVICE_1_WIRING_PI_PIN, ACT_DEVICE_2_WIRING_PI_PIN, ACT_DEVICE_3_WIRING_PI_PIN, ACT_DEVICE_4_WIRING_PI_PIN, ACT_DEVICE_5_WIRING_PI_PIN, ACT_DEVICE_6_WIRING_PI_PIN, ACT_DEVICE_7_WIRING_PI_PIN, ACT_DEVICE_8_WIRING_PI_PIN, ACT_DEVICE_9_WIRING_PI_PIN, ACT_DEVICE_10_WIRING_PI_PIN, ACT_DEVICE_11_WIRING_PI_PIN, ACT_DEVICE_12_WIRING_PI_PIN };

/**
 * @brief Construct a new Action Tracer:: Action Tracer:: Action Tracer object
 *
 */
ActionTracer::ActionTracer::ActionTracer() {
}

// Overload the copy constructor
/**
 * @brief Construct a new Action Tracer:: Action Tracer:: Action Tracer object
 *
 * @param other
 */
ActionTracer::ActionTracer::ActionTracer( const ActionTracer &other ) {}

// Overload the move constructor
/**
 * @brief Construct a new Action Tracer:: Action Tracer:: Action Tracer object
 *
 * @param other
 */
ActionTracer::ActionTracer::ActionTracer( ActionTracer &&other ) {}

// Overload the copy assignment operator
/**
 * @brief Construct a new Action Tracer:: Action Tracer:: Action Tracer object
 *
 * @param other
 * @return ActionTracer&
 */
ActionTracer::ActionTracer &ActionTracer::ActionTracer::operator=( const ActionTracer &other ) {}

/**
 * @brief Construct a new Action Tracer:: Action Tracer:: Action Tracer object
 *
 * @param other
 * @return ActionTracer&
 */
ActionTracer::ActionTracer &ActionTracer::ActionTracer::operator=( ActionTracer &&other ) {}

/**
 * @brief Destroy the Action Tracer:: Action Tracer:: Action Tracer object
 *
 */
ActionTracer::ActionTracer::~ActionTracer() {
}

/**
 * @brief Start collecting data from the Action Tracer Device and deliver it to the API and forward it to server if specified.
 *
 */
void ActionTracer::ActionTracer::start() {
	// This will confirm that the server or listener is ready to receive data and will start sending data packets.
}

/**
 * @brief Stop collecting data from the Action Tracer Device and stop delivering it to the API and server if specified.
 *
 */
void ActionTracer::ActionTracer::stop() {
	// This will stop the Action Device from collecting data and closing all the connections
}

/**
 * @brief Pause collecting data from the Action Tracer Device and pause delivering it to the API and server if specified.
 *
 */
void ActionTracer::ActionTracer::pause() {
	// Temporarily pause packet sending. Do not close receivers
	// Send pause signal message
}

/**
 * @brief Resume collecting data from the Action Tracer Device and resume delivering it to the API and server if specified.
 *
 */
void ActionTracer::ActionTracer::resume() {
	// Continue sending data to clients.
	// Start by sending receive message
}

/**
 * @brief Reset the Action Tracer Device to default startup state.
 *
 */
void ActionTracer::ActionTracer::reset() {
}

/**
 * @brief Initialise the sensors being used for the program. This will take time depending on how many devices are being initialized.
 *
 */
void ActionTracer::ActionTracer::initialize( int16_t device_sum ) {
	// Based on ACT_# we can OR to find which device have been selected.
	// First we check 0, then (in do-while) 1 - 13 and in the control loop, init, set rate and work.

	if( device_sum & ACT_0 ) {
		// Initialize device 0. Since we know the device number, we know the pin it's conencted to.
	}
}

/**
 * @brief Map ACT device and set its internal state to the desired state. This state is it's Wiring Pi pin and its identifier
 *
 * @param rate
 */
void ActionTracer::ActionTracer::map_device( uint16_t ACT_device, uint16_t body_part ) {
	// Add a new Tracerpoint device to the list of devices in use.
	// Create Tracerpoint
	TracePoint *temp_device = new TracePoint();

	// Set the device's pin number
	temp_device->set_pin_number( _get_ACT_device_pin( ACT_device ) );

	// Set the device's identifier
	temp_device->set_identifier( _get_body_identifier( body_part ) );

	// Add device to the list of devices waiting to be initialized.
	_devices_waiting_for_use.push_back( temp_device );
}

void ActionTracer::ActionTracer::set_fifo_rate( uint8_t device, uint8_t ) {
}

uint8_t ActionTracer::ActionTracer::get_fifo_rate( uint8_t device ) const {
	return 1;
}

/**
 * @brief Set the sample rate for all the devices in use to a single given rate.
 *  @param sample_rate The sample rate to set all devices to.
 * @return Nothing
 */
void ActionTracer::ActionTracer::set_sample_rate( uint8_t sample_rate ) {
	// For each deviece in use in_devices_in_use, set the sample rate to the given sample rate.
	for( auto &device : _devices_in_use ) {
		device->set_sample_rate( 2 );
	}
}

uint8_t ActionTracer::ActionTracer::get_sample_rate() const {
	return _act_sample_rate;
}

uint16_t ActionTracer::ActionTracer::_get_body_identifier( uint16_t code ) {
	if( code == ACT_BODY_WAIST ) {
		return ACT_BODY_WAIST;
	} else

		if( code == ACT_BODY_RIGHT_BICEP ) {
		return ACT_BODY_RIGHT_BICEP;
	} else

		if( code == ACT_BODY_RIGHT_FOREARM ) {
		return ACT_BODY_RIGHT_FOREARM;
	} else

		if( code == ACT_BODY_RIGHT_HAND ) {
		return ACT_BODY_RIGHT_HAND;
	} else

		if( code == ACT_BODY_LEFT_BICEP ) {
		return ACT_BODY_LEFT_BICEP;
	} else

		if( code == ACT_BODY_LEFT_FOREARM ) {
		return ACT_BODY_LEFT_FOREARM;
	} else

		if( code == ACT_BODY_LEFT_HAND ) {
		return ACT_BODY_LEFT_HAND;
	} else

		if( code == ACT_BODY_CHEST ) {
		return ACT_BODY_CHEST;
	} else

		if( code == ACT_BODY_HEAD ) {
		return ACT_BODY_HEAD;
	} else

		if( code == ACT_BODY_RIGHT_THIGH ) {
		return ACT_BODY_RIGHT_THIGH;
	} else

		if( code == ACT_BODY_RIGHT_KNEE ) {
		return ACT_BODY_RIGHT_KNEE;
	} else

		if( code == ACT_BODY_RIGHT_FOOT ) {
		return ACT_BODY_RIGHT_FOOT;
	} else

		if( code == ACT_BODY_LEFT_THIGH ) {
		return ACT_BODY_LEFT_THIGH;
	} else

		if( code == ACT_BODY_LEFT_KNEE ) {
		return ACT_BODY_LEFT_KNEE;
	} else

		if( code == ACT_BODY_LEFT_FOOT ) {
		return ACT_BODY_LEFT_FOOT;
	} else

		if( code == ACT_BODY_RIGHT_HIP ) {
		return ACT_BODY_RIGHT_HIP;
	} else

		if( code == ACT_BODY_LEFT_HIP ) {
		return ACT_BODY_LEFT_HIP;
	} else {
		throw std::invalid_argument( "Received a body part identifer that is not defined." );
		return;
	}
}

uint16_t ActionTracer::ActionTracer::_get_ACT_device_pin( uint16_t ACT_device ) {
	if( ACT_0 ) {
		return ACT_0;
	} else

		if( ACT_1 ) {
		return ACT_1;
	} else

		if( ACT_2 ) {
		return ACT_2;
	} else

		if( ACT_3 ) {
		return ACT_3;
	} else

		if( ACT_4 ) {
		return ACT_4;
	} else

		if( ACT_5 ) {
		return ACT_5;
	} else

		if( ACT_6 ) {
		return ACT_6;
	} else

		if( ACT_7 ) {
		return ACT_7;
	} else

		if( ACT_8 ) {
		return ACT_8;
	} else

		if( ACT_9 ) {
		return ACT_9;
	} else

		if( ACT_10 ) {
		return ACT_10;
	} else

		if( ACT_11 ) {
		return ACT_11;
	} else

		if( ACT_12 ) {
		return ACT_12;
	} else {
		throw std::invalid_argument( "Received an ACT device identifier that is not defined." );
		return;
	}
}

void ActionTracer::ActionTracer::show_body() {
	// Show the body being used and which ACT_DEVICE_# is connected

	printf( "  R             +---+                L  \n" );
	printf( "  R             | %-2i|                L  \n", _devices_in_use[8]->get_pin_number() );
	printf( "  R             +- -+                L  \n" );
	printf( "  R            +--%-2i-+               L  \n", _devices_in_use[7]->get_pin_number() );
	printf( "  R         %2i/  | |  \\%-2i            L  \n", _devices_in_use[1]->get_pin_number(), _devices_in_use[4]->get_pin_number() );
	printf( "  R          +   | |   +-%2i-+%2i<     L  \n", _devices_in_use[5]->get_pin_number(), _devices_in_use[6]->get_pin_number() );
	printf( "  R       %2i/    |%1i|                 L  \n", _devices_in_use[2]->get_pin_number(), _devices_in_use[0]->get_pin_number() ); // 0 is the center;
	printf( "  R    >%2i-+  %2i/   \\%-2i              L  \n", _devices_in_use[3]->get_pin_number(), _devices_in_use[15]->get_pin_number(), _devices_in_use[16]->get_pin_number() );
	printf( "  R             |   |                L  \n" );
	printf( "  R            %2i   %-2i               L  \n", _devices_in_use[9]->get_pin_number(), _devices_in_use[12]->get_pin_number() );
	printf( "  R             +   +                L  \n" );
	printf( "  R            %2i   %-2i               L  \n", _devices_in_use[10]->get_pin_number(), _devices_in_use[13]->get_pin_number() );
	printf( "  R             |   |                L  \n" );
	printf( "  R             +   +                L  \n" );
	printf( "  R          %2i/     \\%-2i             L  \n", _devices_in_use[11]->get_pin_number(), _devices_in_use[14]->get_pin_number() );
}