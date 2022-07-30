#include "action_tracer.h"

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
void ActionTracer::ActionTracer::initialize(int16_t device_sum) {
    // Based on ACT_# we can OR to find which device have been selected.
    // First we check 0, then (in do-while) 1 - 13 and in the control loop, init, set rate and work. 
}


void ActionTracer::ActionTracer::set_fifo_rate(int16_t device, uint8_t){
    
}

void ActionTracer::ActionTracer::get_fifo_rate(int16_t device){

}

void ActionTracer::ActionTracer::set_sample_rate(int8_t sample_rate){
    
}

void ActionTracer::ActionTracer::get_sample_rate() const{
    return _act_sample_rate;
}