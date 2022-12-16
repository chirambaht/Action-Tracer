#include "action_tracer.h"

#include "timer.hpp"

#include <algorithm>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <unistd.h>

#ifdef ON_PI
	#include <wiringPi.h>
#else
	#include "wiringPi.h"
#endif

const uint8_t PI_ORDER[13] = { ACT_DEVICE_0_WIRING_PI_PIN, ACT_DEVICE_1_WIRING_PI_PIN, ACT_DEVICE_2_WIRING_PI_PIN,
	ACT_DEVICE_3_WIRING_PI_PIN, ACT_DEVICE_4_WIRING_PI_PIN, ACT_DEVICE_5_WIRING_PI_PIN, ACT_DEVICE_6_WIRING_PI_PIN,
	ACT_DEVICE_7_WIRING_PI_PIN, ACT_DEVICE_8_WIRING_PI_PIN, ACT_DEVICE_9_WIRING_PI_PIN, ACT_DEVICE_10_WIRING_PI_PIN,
	ACT_DEVICE_11_WIRING_PI_PIN, ACT_DEVICE_12_WIRING_PI_PIN };

void ActionTracer::ActionTracer::_data_transmission_thread( Communication::Supervisor *new_super, bool *data_in,
	bool *thread_run ) {
	printf( "Data transmission thread started\n" );

	while( *thread_run ) {
		try {
			if( *data_in ) {
				new_super->send_packet();
				*data_in = false;
			}
		} catch( std::exception &e ) {
			printf( "Exception in data transmission thread: %s\n Probably the client "
					"disconnected\n",
				e.what() );
		}
	}
	printf( "Data transmission thread stopped\n" );
}

void ActionTracer::ActionTracer::_client_manager_thread( Communication::Supervisor *new_super, bool *data_in,
	bool *thread_run ) {
	printf( "Client manager thread started\n" );

	int connected_clients = 0;
	while( *thread_run ) {
		if( new_super->get_ready() && connected_clients < MAX_CLIENTS ) {
			new_super->initialize( true );
			connected_clients++;
		}
	}
	printf( "Client manager thread stopped\n" );
}

/**
 * @brief The main method that runs the I2C communication with the action
 * devices and collects the data.
 *
 * @return void*
 */
void ActionTracer::ActionTracer::_data_collection_thread( Communication::Supervisor *new_super, bool *run_status,
	bool *data_in, bool *thread_run ) {
	printf( "Data collection thread started\n" );

	bool loading = false;

	while( *thread_run ) {
		while( *run_status ) {
			for( uint8_t i = 0; i < MAX_ACT_DEVICES; i++ ) {
				if( _devices_in_use[i]->is_active() ) {
					_data_package_action[i] = _devices_in_use[i]->read_data_action( 1 );
					if( loading || *data_in == false ) {
						new_super->load_packet( _data_package_action[i] );
						loading = true;
					}
				}
			}

			if( loading ) {
				*data_in = true;
				loading	 = false;
			}
		}
	}
	printf( "Data collection thread stopped\n" );
}

/**
 * @brief Construct a new Action Tracer:: Action Tracer:: Action Tracer object
 *
 */
ActionTracer::ActionTracer::ActionTracer() {
	for( int i = 0; i < MAX_ACT_DEVICES; i++ ) {
		_devices_in_use[i]		= new TracePoint();
		_data_package_action[i] = new ActionDataPackage();
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
// ActionTracer::ActionTracer &ActionTracer::ActionTracer::operator=( const
// ActionTracer &other ) {}

/**
 * @brief Construct a new Action Tracer:: Action Tracer:: Action Tracer object
 *
 * @param other
 * @return ActionTracer&
 */
// ActionTracer::ActionTracer &ActionTracer::ActionTracer::operator=(
// ActionTracer &&other ) {}

/**
 * @brief Destroy the Action Tracer:: Action Tracer:: Action Tracer object
 *
 */
ActionTracer::ActionTracer::~ActionTracer() {}

/**
 * @brief Start collecting data from the Action Tracer Device and deliver it to
 * the API and forward it to server if specified.
 *
 */
void ActionTracer::ActionTracer::start() {
	// This will confirm that the server or listener is ready to receive data and
	// will start sending data packets. Set the running flag to true

	if( _running || !_paused ) {
		return;
	}

	// First we need a cue to sync the cameras and let users know when the device is about to start collecting data
	// This will be a 3 second countdown
	for( int i = 3; i > 0; i-- ) {
		_turn_off_all_devices();
		delay( 500 );
		_turn_on_all_devices();
		delay( 500 );
	}
	_turn_off_all_devices();
	delay( 1000 ); // Final pause before starting

	_running = true;
	_paused	 = false;
}

/**
 * @brief Stop collecting data from the Action Tracer Device and stop delivering
 * it to the API and server if specified.
 *
 */
void ActionTracer::ActionTracer::stop() {
	// This will stop the Action Device from collecting data and closing all the
	// connections First pause the device, then do everything else that the pause
	// does not do
	if( !_paused ) {
		pause();
	}
	_running = false;

	_supervisor->disconnect();

	// Stop the threads
	_thread_running_client_manager	  = false;
	_thread_running_data_collection	  = false;
	_thread_running_data_transmission = false;

	// Join the threads
	_thread_client_manager.join();
	_thread_data_collection.join();
	_thread_data_transmission.join();

	// Set the running flag to false

	_turn_off_all_devices();
}

/**
 * @brief Pause collecting data from the Action Tracer Device and pause
 * delivering it to the API and server if specified.
 *
 */
void ActionTracer::ActionTracer::pause() {
	// Temporarily pause packet sending. Do not close receivers
	_paused		= true;
	_data_ready = false;
}

/**
 * @brief Resume collecting data from the Action Tracer Device and resume
 * delivering it to the API and server if specified.
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
	if( _running ) {
		// Stop the device
		stop();
	}
	this->show_body(); // Removable for testing purposes

	// Start threads
}

/**
 * @brief Initialise the sensors being used for the program. This will take time
 * depending on how many devices are being initialized. It will go through each
 * device that has been mapped and intialises it.
 * @param device_map
 * @returns Nothing
 * @throws BAD_MAPPING When the divece has not correctly been mapped.
 * @throws INVALID_SAMPLE_RATE The device has been passed an incorrcet sample
 * rate.
 */
void ActionTracer::ActionTracer::initialize() {
	_supervisor = new Communication::Supervisor();
	_supervisor->initialize( false );

	_thread_running_client_manager	  = true;
	_thread_running_data_collection	  = true;
	_thread_running_data_transmission = true;

	_thread_client_manager	  = std::thread( &ActionTracer::_client_manager_thread, this, _supervisor, &_data_ready,
		   &_thread_running_client_manager );
	_thread_data_transmission = std::thread( &ActionTracer::_data_transmission_thread, this, _supervisor, &_data_ready,
		&_thread_running_data_transmission );

	_turn_off_all_devices();

	for( auto &device : _devices_waiting_for_use ) {
		if( device == nullptr ) {
			continue;
		}
		device->initialize( device->get_pin_number(), device->get_identifier() );

		// Add the devices to the list of devices in use in correct spot
		_devices_in_use[_get_body_identifier( device->get_identifier() )] = device;
	}

	_thread_data_collection = std::thread( &ActionTracer::_data_collection_thread, this, _supervisor, &_running,
		&_data_ready, &_thread_running_data_collection );
}

/**
 * @brief Map ACT device and set its internal state to the desired state. This
 * state is it's Wiring Pi pin and its identifier. Only takes one device and
 * part at a time.
 * @param ACT_device The device to be mapped.
 * @param body_part The body part that the device is being mapped to.
 */
void ActionTracer::ActionTracer::map_device( uint16_t ACT_device, uint16_t body_part ) {
	// Create Tracerpoint
	TracePoint *temp_device = new TracePoint();
	uint8_t		temp_pin	= _get_ACT_device_pin( ACT_device );

	// Add a new Tracerpoint device to the list of devices in use after validating
	// mapping
	_validate_mapping( temp_pin, body_part );

	// Set the device's pin number
	temp_device->set_pin_number( temp_pin );

	// Set the device's identifier
	temp_device->set_identifier( body_part );

	// We will temporarily disable calibration to speed up this process
	temp_device->set_calibrate( true, 250 );

	// Add device to the list of devices waiting to be initialized.
	_devices_waiting_for_use.push_back( temp_device );
}

void ActionTracer::ActionTracer::set_fifo_rate( uint8_t device, uint8_t rate ) {
	_devices_waiting_for_use[device]->set_sample_rate( rate );
}

void ActionTracer::ActionTracer::set_fifo_rate( uint8_t rate ) {
	for( auto &device : _devices_waiting_for_use ) {
		device->set_sample_rate( rate );
	}
}

uint8_t ActionTracer::ActionTracer::get_fifo_rate( uint8_t device ) const { return 1; }

/**
 * @brief Set the sample rate for all the devices in use to a single given rate.
 *  @param sample_rate The sample rate to set all devices to.
 * @return Nothing
 */
void ActionTracer::ActionTracer::set_sample_rate( uint8_t sample_rate ) {
	// For each deviece in use in_devices_in_use, set the sample rate to the given
	// sample rate.
	_act_sample_rate = sample_rate;

	for( auto &device : _devices_waiting_for_use ) {
		device->set_sample_rate( sample_rate );
	}
}

/**
 * @brief Get the general device sample rate that is being used by all devices.
 *  @return The sample rate of all devices in use.
 */
uint8_t ActionTracer::ActionTracer::get_sample_rate() const { return _act_sample_rate; }

/**
 * @brief Finds a given body part identifier and returns it.
 *  @param body_part_code The body part code to validate and return.
 * @return Identifier for the body part code passed in
 * @throws INVALID_ARGUMENT When the body part code is not valid.
 */
uint16_t ActionTracer::ActionTracer::_get_body_identifier( uint16_t body_part_code ) {
	if( body_part_code == ACT_BODY_WAIST ) {
		return 0;
	} else if( body_part_code == ACT_BODY_RIGHT_BICEP ) {
		return 1;
	} else if( body_part_code == ACT_BODY_RIGHT_FOREARM ) {
		return 2;
	} else if( body_part_code == ACT_BODY_RIGHT_HAND ) {
		return 3;
	} else if( body_part_code == ACT_BODY_LEFT_BICEP ) {
		return 4;
	} else if( body_part_code == ACT_BODY_LEFT_FOREARM ) {
		return 5;
	} else if( body_part_code == ACT_BODY_LEFT_HAND ) {
		return 6;
	} else if( body_part_code == ACT_BODY_CHEST ) {
		return 7;
	} else if( body_part_code == ACT_BODY_HEAD ) {
		return 8;
	} else if( body_part_code == ACT_BODY_RIGHT_THIGH ) {
		return 9;
	} else if( body_part_code == ACT_BODY_RIGHT_KNEE ) {
		return 10;
	} else if( body_part_code == ACT_BODY_RIGHT_FOOT ) {
		return 11;
	} else if( body_part_code == ACT_BODY_LEFT_THIGH ) {
		return 12;
	} else if( body_part_code == ACT_BODY_LEFT_KNEE ) {
		return 13;
	} else if( body_part_code == ACT_BODY_LEFT_FOOT ) {
		return 14;
	} else if( body_part_code == ACT_BODY_RIGHT_HIP ) {
		return 15;
	} else if( body_part_code == ACT_BODY_LEFT_HIP ) {
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
	if( ACT_device == ACT_0 ) {
		return ACT_DEVICE_0_WIRING_PI_PIN;
	} else if( ACT_device == ACT_1 ) {
		return ACT_DEVICE_1_WIRING_PI_PIN;
	} else if( ACT_device == ACT_2 ) {
		return ACT_DEVICE_2_WIRING_PI_PIN;
	} else if( ACT_device == ACT_3 ) {
		return ACT_DEVICE_3_WIRING_PI_PIN;
	} else if( ACT_device == ACT_4 ) {
		return ACT_DEVICE_4_WIRING_PI_PIN;
	} else if( ACT_device == ACT_5 ) {
		return ACT_DEVICE_5_WIRING_PI_PIN;
	} else if( ACT_device == ACT_6 ) {
		return ACT_DEVICE_6_WIRING_PI_PIN;
	} else if( ACT_device == ACT_7 ) {
		return ACT_DEVICE_7_WIRING_PI_PIN;
	} else if( ACT_device == ACT_8 ) {
		return ACT_DEVICE_8_WIRING_PI_PIN;
	} else if( ACT_device == ACT_9 ) {
		return ACT_DEVICE_9_WIRING_PI_PIN;
	} else if( ACT_device == ACT_10 ) {
		return ACT_DEVICE_10_WIRING_PI_PIN;
	} else if( ACT_device == ACT_11 ) {
		return ACT_DEVICE_11_WIRING_PI_PIN;
	} else if( ACT_device == ACT_12 ) {
		return ACT_DEVICE_12_WIRING_PI_PIN;
	} else {
		throw std::invalid_argument( "Received an ACT device identifier that is not defined." );
		return 0xFF;
	}
}

void ActionTracer::ActionTracer::show_body() {
	// Show the body being used and which ACT_DEVICE_# is connected
	printf( "Find below the ACT device connections. This is in reference to the "
			"Action Tracer Pi Hat Board\n\n" );
	printf( "  R             +---+                L  \n" );
	printf( "  R             | %-2s|                L  \n",
		_devices_in_use[8]->get_act_pin_number_as_string().c_str() );
	printf( "  R             +- -+                L  \n" );
	printf( "  R            +--%-2s-+               L  \n",
		_devices_in_use[7]->get_act_pin_number_as_string().c_str() );
	printf( "  R         %2s/  | |  \\%-2s            L  \n",
		_devices_in_use[1]->get_act_pin_number_as_string().c_str(),
		_devices_in_use[4]->get_act_pin_number_as_string().c_str() );
	printf( "  R          +   | |   +-%2s-+%2s<     L  \n", _devices_in_use[5]->get_act_pin_number_as_string().c_str(),
		_devices_in_use[6]->get_act_pin_number_as_string().c_str() );
	printf( "  R       %2s/    |%1s|                 L  \n", _devices_in_use[2]->get_act_pin_number_as_string().c_str(),
		_devices_in_use[0]->get_act_pin_number_as_string().c_str() ); // 0 is the center;
	printf( "  R     >%2s+  %2s/   \\%-2s              L  \n",
		_devices_in_use[3]->get_act_pin_number_as_string().c_str(),
		_devices_in_use[15]->get_act_pin_number_as_string().c_str(),
		_devices_in_use[16]->get_act_pin_number_as_string().c_str() );
	printf( "  R             |   |                L  \n" );
	printf( "  R            %2s   %-2s               L  \n", _devices_in_use[9]->get_act_pin_number_as_string().c_str(),
		_devices_in_use[12]->get_act_pin_number_as_string().c_str() );
	printf( "  R             +   +                L  \n" );
	printf( "  R            %2s   %-2s               L  \n",
		_devices_in_use[10]->get_act_pin_number_as_string().c_str(),
		_devices_in_use[13]->get_act_pin_number_as_string().c_str() );
	printf( "  R             |   |                L  \n" );
	printf( "  R             +   +                L  \n" );
	printf( "  R          %2s/     \\%-2s             L  \n\n",
		_devices_in_use[11]->get_act_pin_number_as_string().c_str(),
		_devices_in_use[14]->get_act_pin_number_as_string().c_str() );
}

bool ActionTracer::ActionTracer::_validate_mapping( uint16_t ACT_pin, uint16_t body_part ) {
	std::vector<uint16_t> body_part_codes;
	std::vector<uint16_t> device_codes;

	for( auto &dev : _devices_waiting_for_use ) {
		body_part_codes.push_back( dev->get_identifier() );
		device_codes.push_back( dev->get_pin_number() );
	}

	std::sort( body_part_codes.begin(), body_part_codes.end() );
	std::sort( device_codes.begin(), device_codes.end() );

	// Check if there are duplicate body part codes and return false if there are.
	for( auto i = body_part_codes.begin(); i != body_part_codes.end(); ++i ) {
		if( std::find( i + 1, body_part_codes.end(), *i ) != body_part_codes.end() ) {
			throw std::invalid_argument( "Bad mapping! This body part is already defined." );
			return false;
		}
	}

	// Check if there are duplicate body part codes and return false if there are.
	for( auto i = device_codes.begin(); i != device_codes.end(); ++i ) {
		if( std::find( i + 1, device_codes.end(), *i ) != device_codes.end() ) {
			throw std::invalid_argument( "Bad mapping! This ACT device is already in use." );
			return false;
		}
	}

	return true;
}

bool ActionTracer::ActionTracer::_turn_off_all_devices() {
	uint8_t ALL_ACT_DEVICE_PINS[MAX_ACT_DEVICES] = { ACT_DEVICE_0_WIRING_PI_PIN, ACT_DEVICE_1_WIRING_PI_PIN,
		ACT_DEVICE_2_WIRING_PI_PIN, ACT_DEVICE_3_WIRING_PI_PIN, ACT_DEVICE_4_WIRING_PI_PIN, ACT_DEVICE_5_WIRING_PI_PIN,
		ACT_DEVICE_6_WIRING_PI_PIN, ACT_DEVICE_7_WIRING_PI_PIN, ACT_DEVICE_8_WIRING_PI_PIN, ACT_DEVICE_9_WIRING_PI_PIN,
		ACT_DEVICE_10_WIRING_PI_PIN, ACT_DEVICE_11_WIRING_PI_PIN, ACT_DEVICE_12_WIRING_PI_PIN };

	for( int i = 0; i < MAX_ACT_DEVICES; i++ ) {
		digitalWrite( ALL_ACT_DEVICE_PINS[i], LOW );
	}
	return true;
}

bool ActionTracer::ActionTracer::_turn_on_all_devices() {
	uint8_t ALL_ACT_DEVICE_PINS[MAX_ACT_DEVICES] = { ACT_DEVICE_0_WIRING_PI_PIN, ACT_DEVICE_1_WIRING_PI_PIN,
		ACT_DEVICE_2_WIRING_PI_PIN, ACT_DEVICE_3_WIRING_PI_PIN, ACT_DEVICE_4_WIRING_PI_PIN, ACT_DEVICE_5_WIRING_PI_PIN,
		ACT_DEVICE_6_WIRING_PI_PIN, ACT_DEVICE_7_WIRING_PI_PIN, ACT_DEVICE_8_WIRING_PI_PIN, ACT_DEVICE_9_WIRING_PI_PIN,
		ACT_DEVICE_10_WIRING_PI_PIN, ACT_DEVICE_11_WIRING_PI_PIN, ACT_DEVICE_12_WIRING_PI_PIN };

	for( int i = 0; i < MAX_ACT_DEVICES; i++ ) {
		digitalWrite( ALL_ACT_DEVICE_PINS[i], HIGH );
	}
	return true;
}

uint8_t ActionTracer::ActionTracer::get_connected_clients() const { return _supervisor->get_connected_clients(); }

ActionTracer::Communication::ActionServer *ActionTracer::ActionTracer::get_server() {
	return _supervisor->get_server();
}

/**
 * @brief Get the packet number that the supervisor is sending
 *
 * @return uint32_t current packet number being sent
 */
uint32_t ActionTracer::ActionTracer::get_packet_number() const {
	if( _supervisor == nullptr ) {
		return 0;
	}

	return _supervisor->get_packet_number();
}
