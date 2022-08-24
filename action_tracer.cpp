#include "action_tracer.h"

#include <algorithm>
#include <cstdio>
#include <stdexcept>
#include <unistd.h>

#ifdef ON_PI
	#include <wiringPi.h>
#else
	#include "wiringPi.h"
#endif

const uint8_t PI_ORDER[13] = { ACT_DEVICE_0_WIRING_PI_PIN, ACT_DEVICE_1_WIRING_PI_PIN, ACT_DEVICE_2_WIRING_PI_PIN, ACT_DEVICE_3_WIRING_PI_PIN, ACT_DEVICE_4_WIRING_PI_PIN, ACT_DEVICE_5_WIRING_PI_PIN,
	ACT_DEVICE_6_WIRING_PI_PIN, ACT_DEVICE_7_WIRING_PI_PIN, ACT_DEVICE_8_WIRING_PI_PIN, ACT_DEVICE_9_WIRING_PI_PIN, ACT_DEVICE_10_WIRING_PI_PIN, ACT_DEVICE_11_WIRING_PI_PIN,
	ACT_DEVICE_12_WIRING_PI_PIN };
void		 *_data_collection_thread( void *arg );

void ActionTracer::ActionTracer::data_transmission_thread( Communication::Supervisor *new_super, bool *data_in ) {
	if ( !new_super->get_ready() ) {
		printf( "Waiting for a client to connect...\n" );
		new_super->initialize( true );
	}
	printf( "Action Server is now running" );
	int packet_count = 0;
	while ( true ) {
		if ( *data_in ) {
			if ( packet_count % 50 == 0 ) {
				printf( "Sent a packet\n" );
			}

			packet_count++;
			new_super->send_packet();
			*data_in = !*data_in;
		}
	}
}

/**
 * @brief The main method that runs the I2C communication with the action devices and collects the data.
 *
 * @return void*
 */
void *ActionTracer::ActionTracer::data_collection_thread() {
	while ( 1 ) {
		while ( _running != false ) {
			for ( uint8_t i = 0; i < MAX_ACT_DEVICES; i++ ) {
				if ( _devices_in_use[i]->is_active() ) {
					_data_package_action[i] = _devices_in_use[i]->read_data_action( 1 );
				}
			}

			// _supervisor->send_packet(); // Send the data to all clients
			// wait for 1 ms
			usleep( 1000 );
			// Send data to clients
			_data_ready = true;

			// When the data has been sent, the new packet can be loaded and sent
			if ( _data_ready == false ) {
				for ( uint8_t i = 0; i < MAX_ACT_DEVICES; i++ ) {
					if ( _devices_in_use[i]->is_active() ) {
						_supervisor->load_packet( _data_package_action[i] ); // Immidiately send the data to all clients
					}
				}
				_supervisor->send_packet();
			}
		}
	}
}

/**
 * @brief The main method that runs the data transmission to a server or client.
 *
 * @return void*
 */
void *ActionTracer::ActionTracer::data_sending_thread( void *arg ) {
	// First connect to clients via the packager

	while ( 1 ) {
		if ( !_paused && _supervisor->get_ready() && _data_ready ) {
			_supervisor->send_packet();
		}
	}
}

/**
 * @brief Construct a new Action Tracer:: Action Tracer:: Action Tracer object
 *
 */
ActionTracer::ActionTracer::ActionTracer() {
	for ( int i = 0; i < MAX_ACT_DEVICES; i++ ) {
		_devices_in_use[i] = new TracePoint();
	}
}

// Overload the copy constructor
/**
 * @brief Construct a new Action Tracer:: Action Tracer:: Action Tracer object
 *
 * @param other
 */
// ActionTracer::ActionTracer::ActionTracer( const ActionTracer &other ) {}

// Overload the move constructor
/**
 * @brief Construct a new Action Tracer:: Action Tracer:: Action Tracer object
 *
 * @param other
 */
// ActionTracer::ActionTracer::ActionTracer( ActionTracer &&other ) {}

// Overload the copy assignment operator
/**
 * @brief Construct a new Action Tracer:: Action Tracer:: Action Tracer object
 *
 * @param other
 * @return ActionTracer&
 */
// ActionTracer::ActionTracer &ActionTracer::ActionTracer::operator=( const ActionTracer &other ) {}

/**
 * @brief Construct a new Action Tracer:: Action Tracer:: Action Tracer object
 *
 * @param other
 * @return ActionTracer&
 */
// ActionTracer::ActionTracer &ActionTracer::ActionTracer::operator=( ActionTracer &&other ) {}

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
	// Set the running flag to true
	_running = true;
	_paused	 = false;

	data_collection_thread();
}

/**
 * @brief Stop collecting data from the Action Tracer Device and stop delivering it to the API and server if specified.
 *
 */
void ActionTracer::ActionTracer::stop() {
	// This will stop the Action Device from collecting data and closing all the connections
	// First pause the device, then do everything else that the pause does not do
	if ( !_paused ) {
		pause();
	}

	// Set the running flag to false
	_running = false;
	_turn_off_all_devices();

	_supervisor->disconnect();
	_data_ready = false;
}

/**
 * @brief Pause collecting data from the Action Tracer Device and pause delivering it to the API and server if specified.
 *
 */
void ActionTracer::ActionTracer::pause() {
	// Temporarily pause packet sending. Do not close receivers
	_paused		= true;
	_data_ready = false;
}

/**
 * @brief Resume collecting data from the Action Tracer Device and resume delivering it to the API and server if specified.
 *
 */
void ActionTracer::ActionTracer::resume() {
	// Continue sending data to clients.
	_paused = false;
	// Start by sending receive message
}

/**
 * @brief Reset the Action Tracer Device to default startup state.
 *
 */
void ActionTracer::ActionTracer::reset() {
	// Check if the device is running
	if ( _running ) {
		// Stop the device
		stop();
	}
	this->show_body(); // Removable for testing purposes

	// Start threads
}

/**
 * @brief Initialise the sensors being used for the program. This will take time depending on how many devices are being initialized. It will go through each device that has been mapped and intialises
 * it.
 * @param device_map
 * @returns Nothing
 * @throws BAD_MAPPING When the divece has not correctly been mapped.
 * @throws INVALID_SAMPLE_RATE The device has been passed an incorrcet sample rate.
 */
void ActionTracer::ActionTracer::initialize() {
	_supervisor = new Communication::Supervisor();

	_supervisor->initialize( false );
	bool *ptr_rdy = &_data_ready;

	std::thread data_transmission( &ActionTracer::data_transmission_thread, this, _supervisor, ptr_rdy );
	data_transmission.detach();

	_turn_off_all_devices();

	for ( auto &device : _devices_waiting_for_use ) {
		if ( device == nullptr ) {
			continue;
		}
		device->initialize( device->get_pin_number(), device->get_identifier() );

		// Add the devices to the list of devices in use in correct spot
		_devices_in_use[_get_body_identifier( device->get_identifier() )] = device;
	}

	this->show_body(); // Removable for testing purposes

	// Start threads
}

/**
 * @brief Map ACT device and set its internal state to the desired state. This state is it's Wiring Pi pin and its identifier. Only takes one device and part at a time.
 * @param ACT_device The device to be mapped.
 * @param body_part The body part that the device is being mapped to.
 */
void ActionTracer::ActionTracer::map_device( uint16_t ACT_device, uint16_t body_part ) {
	// Create Tracerpoint
	TracePoint *temp_device = new TracePoint();
	uint8_t		temp_pin	= _get_ACT_device_pin( ACT_device );

	// Add a new Tracerpoint device to the list of devices in use after validating mapping
	_validate_mapping( temp_pin, body_part );

	// Set the device's pin number
	temp_device->set_pin_number( temp_pin );

	// Set the device's identifier
	temp_device->set_identifier( body_part );

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
void ActionTracer::ActionTracer::set_sample_rate( uint8_t sample_rate )

{
	// For each deviece in use in_devices_in_use, set the sample rate to the given sample rate.
	for ( auto &device : _devices_in_use ) {
		device->set_sample_rate( 2 );
	}
}

/**
 * @brief Get the general device sample rate that is being used by all devices.
 *  @return The sample rate of all devices in use.
 */
uint8_t ActionTracer::ActionTracer::get_sample_rate() const {
	return _act_sample_rate;
}

/**
 * @brief Finds a given body part identifier and returns it.
 *  @param body_part_code The body part code to validate and return.
 * @return Identifier for the body part code passed in
 * @throws INVALID_ARGUMENT When the body part code is not valid.
 */
uint16_t ActionTracer::ActionTracer::_get_body_identifier( uint16_t body_part_code ) {
	if ( body_part_code == ACT_BODY_WAIST ) {
		return 0;
	} else if ( body_part_code == ACT_BODY_RIGHT_BICEP ) {
		return 1;
	} else if ( body_part_code == ACT_BODY_RIGHT_FOREARM ) {
		return 2;
	} else if ( body_part_code == ACT_BODY_RIGHT_HAND ) {
		return 3;
	} else if ( body_part_code == ACT_BODY_LEFT_BICEP ) {
		return 4;
	} else if ( body_part_code == ACT_BODY_LEFT_FOREARM ) {
		return 5;
	} else if ( body_part_code == ACT_BODY_LEFT_HAND ) {
		return 6;
	} else if ( body_part_code == ACT_BODY_CHEST ) {
		return 7;
	} else if ( body_part_code == ACT_BODY_HEAD ) {
		return 8;
	} else if ( body_part_code == ACT_BODY_RIGHT_THIGH ) {
		return 9;
	} else if ( body_part_code == ACT_BODY_RIGHT_KNEE ) {
		return 10;
	} else if ( body_part_code == ACT_BODY_RIGHT_FOOT ) {
		return 11;
	} else if ( body_part_code == ACT_BODY_LEFT_THIGH ) {
		return 12;
	} else if ( body_part_code == ACT_BODY_LEFT_KNEE ) {
		return 13;
	} else if ( body_part_code == ACT_BODY_LEFT_FOOT ) {
		return 14;
	} else if ( body_part_code == ACT_BODY_RIGHT_HIP ) {
		return 15;
	} else if ( body_part_code == ACT_BODY_LEFT_HIP ) {
		return 16;
	} else {
		throw std::invalid_argument( "Received a body part identifer that is not defined." );
		return 0xFFFF;
	}
}

/**
 * @brief Finds a given body part identifier and returns it.
 *  @param ACT_device The ACT device code to validate and return.
 * @return Identifier for the device
 * @throws INVALID_ARGUMENT When the body part code is not valid.
 */
uint8_t ActionTracer::ActionTracer::_get_ACT_device_pin( uint16_t ACT_device ) {
	if ( ACT_device == ACT_0 ) {
		return ACT_DEVICE_0_WIRING_PI_PIN;
	} else if ( ACT_device == ACT_1 ) {
		return ACT_DEVICE_1_WIRING_PI_PIN;
	} else if ( ACT_device == ACT_2 ) {
		return ACT_DEVICE_2_WIRING_PI_PIN;
	} else if ( ACT_device == ACT_3 ) {
		return ACT_DEVICE_3_WIRING_PI_PIN;
	} else if ( ACT_device == ACT_4 ) {
		return ACT_DEVICE_4_WIRING_PI_PIN;
	} else if ( ACT_device == ACT_5 ) {
		return ACT_DEVICE_5_WIRING_PI_PIN;
	} else if ( ACT_device == ACT_6 ) {
		return ACT_DEVICE_6_WIRING_PI_PIN;
	} else if ( ACT_device == ACT_7 ) {
		return ACT_DEVICE_7_WIRING_PI_PIN;
	} else if ( ACT_device == ACT_8 ) {
		return ACT_DEVICE_8_WIRING_PI_PIN;
	} else if ( ACT_device == ACT_9 ) {
		return ACT_DEVICE_9_WIRING_PI_PIN;
	} else if ( ACT_device == ACT_10 ) {
		return ACT_DEVICE_10_WIRING_PI_PIN;
	} else if ( ACT_device == ACT_11 ) {
		return ACT_DEVICE_11_WIRING_PI_PIN;
	} else if ( ACT_device == ACT_12 ) {
		return ACT_DEVICE_12_WIRING_PI_PIN;
	} else {
		throw std::invalid_argument( "Received an ACT device identifier that is not defined." );
		return 0xFF;
	}
}

void ActionTracer::ActionTracer::show_body() {
	// Show the body being used and which ACT_DEVICE_# is connected

	printf( "  R             +---+                L  \n" );
	printf( "  R             | %-2s|                L  \n", _devices_in_use[8]->get_pin_number_as_string() );
	printf( "  R             +- -+                L  \n" );
	printf( "  R            +--%-2s-+               L  \n", _devices_in_use[7]->get_pin_number_as_string() );
	printf( "  R         %2s/  | |  \\%-2s            L  \n", _devices_in_use[1]->get_pin_number_as_string(), _devices_in_use[4]->get_pin_number_as_string() );
	printf( "  R          +   | |   +-%2s-+%2s<     L  \n", _devices_in_use[5]->get_pin_number_as_string(), _devices_in_use[6]->get_pin_number_as_string() );
	printf( "  R       %2s/    |%2s|                 L  \n", _devices_in_use[2]->get_pin_number_as_string(), _devices_in_use[0]->get_pin_number_as_string() ); // 0 is the center;
	printf( "  R    >%2s-+  %2s/   \\%-2s              L  \n", _devices_in_use[3]->get_pin_number_as_string(), _devices_in_use[15]->get_pin_number_as_string(),
		_devices_in_use[16]->get_pin_number_as_string() );
	printf( "  R             |   |                L  \n" );
	printf( "  R            %2s   %-2s               L  \n", _devices_in_use[9]->get_pin_number_as_string(), _devices_in_use[12]->get_pin_number_as_string() );
	printf( "  R             +   +                L  \n" );
	printf( "  R            %2s   %-2s               L  \n", _devices_in_use[10]->get_pin_number_as_string(), _devices_in_use[13]->get_pin_number_as_string() );
	printf( "  R             |   |                L  \n" );
	printf( "  R             +   +                L  \n" );
	printf( "  R          %2s/     \\%-2s             L  \n", _devices_in_use[11]->get_pin_number_as_string(), _devices_in_use[14]->get_pin_number_as_string() );
}

bool ActionTracer::ActionTracer::_validate_mapping( uint16_t ACT_pin, uint16_t body_part ) {
	std::vector<uint16_t> body_part_codes;
	std::vector<uint16_t> device_codes;
	for ( auto &dev : _devices_waiting_for_use ) {
		body_part_codes.push_back( dev->get_identifier() );
		device_codes.push_back( dev->get_pin_number() );
	}

	std::sort( body_part_codes.begin(), body_part_codes.end() );
	std::sort( device_codes.begin(), device_codes.end() );

	// Check if there are duplicate body part codes and return false if there are.
	for ( auto i = body_part_codes.begin(); i != body_part_codes.end(); ++i ) {
		if ( std::find( i + 1, body_part_codes.end(), *i ) != body_part_codes.end() ) {
			throw std::invalid_argument( "Bad mapping! This body part is already defined." );
			return false;
		}
	}

	// Check if there are duplicate body part codes and return false if there are.
	for ( auto i = device_codes.begin(); i != device_codes.end(); ++i ) {
		if ( std::find( i + 1, device_codes.end(), *i ) != device_codes.end() ) {
			throw std::invalid_argument( "Bad mapping! This ACT device is already in use." );
			return false;
		}
	}

	return true;
}

bool ActionTracer::ActionTracer::_turn_off_all_devices() {
	uint8_t ALL_ACT_DEVICE_PINS[MAX_ACT_DEVICES] = { ACT_DEVICE_0_WIRING_PI_PIN, ACT_DEVICE_1_WIRING_PI_PIN, ACT_DEVICE_2_WIRING_PI_PIN, ACT_DEVICE_3_WIRING_PI_PIN, ACT_DEVICE_4_WIRING_PI_PIN,
		ACT_DEVICE_5_WIRING_PI_PIN, ACT_DEVICE_6_WIRING_PI_PIN, ACT_DEVICE_7_WIRING_PI_PIN, ACT_DEVICE_8_WIRING_PI_PIN, ACT_DEVICE_9_WIRING_PI_PIN, ACT_DEVICE_10_WIRING_PI_PIN,
		ACT_DEVICE_11_WIRING_PI_PIN, ACT_DEVICE_12_WIRING_PI_PIN };

	for ( int i = 0; i < MAX_ACT_DEVICES; i++ ) {
		digitalWrite( ALL_ACT_DEVICE_PINS[i], LOW );
	}
	return true;
}
