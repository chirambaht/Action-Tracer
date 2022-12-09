#include "tracer_point.h"

#include "calibrator.hpp"
#include "debug_printer.h"

#ifdef ON_PI
#include <wiringPi.h>
#else
#include "wiringPi.h"
#endif

using namespace ActionTracer;

/**
 * @brief Construct a new defult Action Tracer::Trace Point::Trace Point object
 * @constructor
 */
ActionTracer::TracePoint::TracePoint() {
  set_identifier(0);
  set_pin_number(0);
}

/**
 * @brief Construct a new Action Tracer:: TracePoint::Trace Point object.
 * @param identifier Identifier given to the device
 * @param wiring_Pi_pin_number Slave select pin on Raspberry pi according to
 * WiringPi.
 * @constructor
 */
ActionTracer::TracePoint::TracePoint(uint16_t identifier,
                                     uint8_t wiring_Pi_pin_number) {
  set_identifier(identifier);
  set_pin_number(wiring_Pi_pin_number);

  _initialize();
}

void ActionTracer::TracePoint::initialize(uint8_t pin_number,
                                          uint8_t _identifier) {
  printf("Initialising device %d on pin %d\n", _identifier, _pin_number);
  set_pin_number(pin_number);
  set_identifier(_identifier);

  _initialize();
}

void ActionTracer::TracePoint::_initialize() {
  // Check if device has already been initialized
  if (_device_initialized) {
    debugPrintln("Device already initialized\n");
    return;
  }

  pinMode(_pin_number, OUTPUT);

  _device_interrupt_flag = false;

  this->_select_me();
  _device = new MPU6050(0x69);

  debugPrint(_device->testConnection() ? "%d connection successful\n"
                                       : "%d connection failed\n",
             _identifier);

  debugPrintln("Initializing the device as is needed. Identifier = %d\n",
               _identifier);

  _device->initialize();

  // TODO: At this stage an interrupt pin is initialised

  // DMP Initialization

  debugPrint("Initalising DMP\n");
  _device_status = _device->dmpInitialize(_fifo_rate);

  _set_device_offsets();

  if (_device_status == 0) {
    debugPrint("Enabling DMP...");
    _device->setDMPEnabled(true);

    _dmp_ready = true;

    _packet_size = _device->dmpGetFIFOPacketSize();

    debugPrint("Enabled!\n");
  } else {
    debugPrint("Can't initialise DMP\n");
    _dmp_ready = false;
  }

  this->_deselect_me();

#ifdef DEBUG
  this->dump_variables();
#endif

  _device_initialized = true;
}

/**
 * @brief Selects a given MPU6050 node. Must be deselected after use
 * @returns Nothing
 */
void ActionTracer::TracePoint::_select_me() { digitalWrite(_pin_number, HIGH); }

/**
 * @brief Prints out all the data from this device.
 * @returns Nothing
 */
void ActionTracer::TracePoint::dump_variables() {
  // Print out the private variables
  printf("Identifier: %d\n", _identifier);
  printf("Pin number: %d\n", _pin_number);
  printf("Device status: %d\n", _device_status);
  printf("DMP ready: %d\n", _dmp_ready);
  printf("Packet size: %d\n", _packet_size);
  printf("Device interrupt flag: %d\n", _device_interrupt_flag);
}

/**
 * @brief Deselects a given MPU6050 node by turning off it's pin.
 * @return Nothing
 */
void ActionTracer::TracePoint::_deselect_me() {
  digitalWrite(_pin_number, LOW);
}

/**
 * @brief Calls on the selected sensor to identify itself by returning it's
 * identifier. It will physically indicate this by blinking an onboard LED.
 * @return device identifier
 */
uint8_t ActionTracer::TracePoint::identify() {
  // Blink device led
  for (size_t i = 0; i < 5; i++) {
    _select_me();
    delay(250);
    _deselect_me();
    delay(250);
  }

  return _identifier;
}

/**
 * @brief Deselects a given MPU6050 node. Public function to avoid issues.
 */
void ActionTracer::TracePoint::turn_off() { _deselect_me(); }

/**
 * @brief Obtain the data from the sensor. Collects the FIFO packet and extracts
 *the needed data.
 * 	   ================================================================================================
 *	 | Default MotionApps v2.0 42-byte FIFO packet structure: | | | | [QUAT
 *W][      ][QUAT X][      ][QUAT Y][      ][QUAT Z][      ][GYRO X][ ][GYRO Y][
 *] | |   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17
 *18  19  20  21  22  23  | | | | [GYRO Z][      ][ACC X ][      ][ACC Y ][
 *][ACC Z ][      ][      ]                         | |  24  25  26  27  28  29
 *30  31  32  33  34  35  36  37  38  39  40  41                          |
 *	   ================================================================================================
 * @return  Nothing
 */
void ActionTracer::TracePoint::get_data() {
  if (!_dmp_ready) {
    debugPrint("%d: DMP not initialised\n", _identifier);
    this->_deselect_me();
    return;
  }

  this->_select_me();

  _device_interrupt_status = _device->getIntStatus();

  // does the FIFO have data in it?
  if ((_device_interrupt_status & 0x02) < 1) {
    debugPrintln("%d: Interrupt not raised\n", _identifier);
    this->_deselect_me();
    return;
  }

  _fifo_count = _device->getFIFOCount();

  if (_fifo_count >= 1024) {
    // reset so we can continue cleanly
    _device->resetFIFO();

    debugPrint("%d: FIFO overflow!\n", _identifier);
    this->_deselect_me();
    return;
  }

  while (_fifo_count < _packet_size) {
    _fifo_count = _device->getFIFOCount();
    // debugPrintln("%d: MPU interrupt not ready or not enough elements in
    // FIFO\n",
    //              _identifier);
    // this->_deselect_me();
    // return;
  }

  _device->getFIFOBytes(_fifo_buffer, _packet_size);

  /* ================================================================================================
   * | Default MotionApps v2.0 42-byte FIFO packet structure: | | | | [QUAT W][
   ][QUAT X][      ][QUAT Y][      ][QUAT Z][      ][GYRO X][      ][GYRO Y][ ]
   | |   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17
   18  19  20  21  22  23  | | | | [GYRO Z][      ][ACC X ][      ][ACC Y ][
   ][ACC Z ][      ][      ]                         | |  24  25  26  27  28  29
   30  31  32  33  34  35  36  37  38  39  40  41                          |
   * ================================================================================================ */

  // First get dmpQuaternion data as it is pivotal to all work
  _device->dmpGetQuaternion(&_quaternion_packet, _fifo_buffer);

  // Get acceleration data next
  _device->dmpGetAccel(&_acceleration_packet, _fifo_buffer);

  // Get gyroscope data next
  _device->dmpGetGyro(&_gyroscope_packet, _fifo_buffer);

  // Get Temperature
  _temperature_packet = (_device->getTemperature() / 340.0) + 36.53;

  // Add all data to the teapot float packet
  _data_package.data[0] = _quaternion_packet.w; // Max value: 1.0
  _data_package.data[1] = _quaternion_packet.x; // Max Value: 1.0
  _data_package.data[2] = _quaternion_packet.y; // Max Value: 1.0
  _data_package.data[3] = _quaternion_packet.z; // Max Value: 1.0

  _data_package.data[4] =
      (_acceleration_packet.x / 16384.0) * 9.81; // Max Value: 2
  _data_package.data[5] =
      (_acceleration_packet.y / 16384.0) * 9.81; // Max Value: 2
  _data_package.data[6] =
      (_acceleration_packet.z / 16384.0) * 9.81; // Max Value: 2

  _data_package.data[7] = (_gyroscope_packet.x / 16.4); // Max Value: 2000
  _data_package.data[8] = (_gyroscope_packet.y / 16.4); // Max Value: 2000
  _data_package.data[9] = (_gyroscope_packet.z / 16.4); // Max Value: 2000

  _data_package.data[10] = _temperature_packet; // Max Value: 85

  this->_deselect_me();
}

/**
 * @brief Obtain the data from the sensor. This will return a float array based
 * on the requested data defined in the file.
 * @param read_first Collects data first if set to 1 or true. After this, it
 * will return the data
 * @return Pointer to a float array with the data packet
 */
ActionDataPackage *
ActionTracer::TracePoint::read_data_action(int read_first = 0) {
  if (read_first) {
    this->get_data();
  }
  return &_data_package;
}

/**
 * @brief Obtain the data from the sensor. This will return a float array based
 * on the requested data defined in the file.
 * @param read_first Collects data first if set to 1 or true. After this, it
 * will return the data
 * @return Pointer to a float array with the data packet
 */
float *ActionTracer::TracePoint::read_data(int read_first = 0) {
  if (read_first) {
    this->get_data();
  }

  return _complete_float_packet;
}

/**
 * @brief Calibrates the device obtaining the offsets of the sensor.
 * @return Nothing
 */
void ActionTracer::TracePoint::_set_device_offsets() {
  if (!_calibrate) {
    return;
  }

  _calibrate_device(_device,
                    _calibration_samples); // Callibrate the device with the
                                           // given number of samples
}

/**
 * @brief Sets the _calibrate value.
 * @return None
 */
void ActionTracer::TracePoint::set_calibrate(bool in_value) {
  _calibrate = in_value;
}

/**
 * @brief Sets the _calibrate value.
 * @return None
 */
void ActionTracer::TracePoint::set_calibrate(
    bool in_value, uint8_t calibration_samples = 250) {
  _calibrate = in_value;
  _calibration_samples = calibration_samples;
}

/**
 * @brief Gets the number of float packets that will be returned by each device
 * @return size of float array being returned
 */
uint8_t ActionTracer::TracePoint::get_data_packet_size() { return 19; }

/**
 * @brief Sets the device sample rate to the given value if it has not been
 * initialisd yet.
 * @param new_rate Sample rate to set the device to
 * @throws DEVICE_INITIALIZED if the device has been initialised yet meaning it
 * can not be alterd
 */
void ActionTracer::TracePoint::set_sample_rate(uint8_t new_rate) {
  if (_device_initialized) {
    // stderr( "Device is already initialized. Cannot change sample rate." );
    throw _device_initialized;
  }
  _fifo_rate = new_rate;
}

/**
 * @brief Obtains the device status with regard to it being initialized or not.
 * @return True if the device is initialized
 * @return False if the device is not yet initialized
 */
bool ActionTracer::TracePoint::is_active() { return _device_initialized; }

uint16_t ActionTracer::TracePoint::get_pin_number() const {
  return _pin_number;
}

std::string ActionTracer::TracePoint::get_act_pin_number_as_string() const {
  if (!_device_initialized) {
    return "";
  }

  if (_pin_number == ACT_DEVICE_0_WIRING_PI_PIN) {
    return "0";
  } else if (_pin_number == ACT_DEVICE_1_WIRING_PI_PIN) {
    return "1";
  } else if (_pin_number == ACT_DEVICE_2_WIRING_PI_PIN) {
    return "2";
  } else if (_pin_number == ACT_DEVICE_3_WIRING_PI_PIN) {
    return "3";
  } else if (_pin_number == ACT_DEVICE_4_WIRING_PI_PIN) {
    return "4";
  } else if (_pin_number == ACT_DEVICE_5_WIRING_PI_PIN) {
    return "5";
  } else if (_pin_number == ACT_DEVICE_6_WIRING_PI_PIN) {
    return "6";
  } else if (_pin_number == ACT_DEVICE_7_WIRING_PI_PIN) {
    return "7";
  } else if (_pin_number == ACT_DEVICE_8_WIRING_PI_PIN) {
    return "8";
  } else if (_pin_number == ACT_DEVICE_9_WIRING_PI_PIN) {
    return "9";
  } else if (_pin_number == ACT_DEVICE_10_WIRING_PI_PIN) {
    return "10";
  } else if (_pin_number == ACT_DEVICE_11_WIRING_PI_PIN) {
    return "11";
  } else if (_pin_number == ACT_DEVICE_12_WIRING_PI_PIN) {
    return "12";
  }
  return "";
}

void ActionTracer::TracePoint::set_pin_number(uint8_t pin) {
  _pin_number = pin;
  _data_package.device_identifier_contents =
      _data_package.device_identifier_contents | pin;
}

uint16_t ActionTracer::TracePoint::get_identifier() const {
  return _identifier;
}

void ActionTracer::TracePoint::set_identifier(uint16_t identity) {
  _identifier = identity;
  _data_package.device_identifier_contents = _identifier;
}