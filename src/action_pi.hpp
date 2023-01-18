/**
 * This is used to define PI pins for the Action Tracer Suite of Applications. Complies with the Action Connector.
 */
#ifndef ACTION_PI_H
#define ACTION_PI_H

/*
 +-----+-----+---------+------+--Pi Zero-+------+---------+-----+-----+
 | AcT | wPi |   Name  | Mode | Physical | Mode | Name    | wPi | AcT |
 +-----+-----+---------+------+----++----+------+---------+-----+-----+
 |     |     |   3V3   |      |  1 || 2  |      |   5v    |     |     |
 |     |   8 |  Data   |      |  3 || 4  |      |   5v    |     |     |
 |     |   9 |  Clock  |      |  5 || 6  |      |   GND   |     |     |
 |   0 |   7 |         |      |  7 || 8  |      |         | 15  |     |
 |     |     |   GND   |      |  9 || 10 |      |         | 16  |     |
 |   1 |   0 |         |      | 11 || 12 |      |         | 1   |     |
 |   2 |   2 |         |      | 13 || 14 |      |   GND   |     |     |
 |   3 |   3 |         |      | 15 || 16 |      |         | 4   |     |
 |     |     |   3V3   |      | 17 || 18 |      |         | 5   |     |
 |   4 |  12 |         |      | 19 || 20 |      |   GND   |     |     |
 |   5 |  13 |         |      | 21 || 22 |      |         | 6   |     |
 |   6 |  14 |         |      | 23 || 24 |      |         | 10  |     |
 |     |     |   GND   |      | 25 || 26 |      |         | 11  |     |
 |   7 |  30 |         |      | 27 || 28 |      |         | 31  |     |
 |   8 |  21 |         |      | 29 || 30 |      |   GND   |     |     |
 |   9 |  22 |         |      | 31 || 32 |      |         | 26  |     |
 |  10 |  23 |         |      | 33 || 34 |      |   GND   |     |     |
 |  11 |  24 |         |      | 35 || 36 |      |         | 27  | INT |
 |  12 |  25 |         |      | 37 || 38 |      |         | 28  |     |
 |     |     |   GND   |      | 39 || 40 |      |         | 29  |     |
 +-----+-----+---------+------+----++----+------+---------+-----+-----+
 | AcT | wPi |   Name  | Mode | Physical | Mode | Name    | wPi | AcT |
 +-----+-----+---------+------+-Pi Zero--+------+---------+-----+-----+
*/

namespace ActionTracer {
	// int get_pi_location( int locator );
	// int get_action_tracer_device( int device_numeber );

#define ACT_I2C_DATA  8
#define ACT_I2C_CLOCK 9

#define ACT_DEVICE_0  7
#define ACT_DEVICE_1  0
#define ACT_DEVICE_2  2
#define ACT_DEVICE_3  3
#define ACT_DEVICE_4  12
#define ACT_DEVICE_5  13
#define ACT_DEVICE_6  14
#define ACT_DEVICE_7  30
#define ACT_DEVICE_8  21
#define ACT_DEVICE_9  22
#define ACT_DEVICE_10 23
#define ACT_DEVICE_11 24
#define ACT_DEVICE_12 25

#define ACT_DEVICE_0_INT 27

	// An array with all the Pi pins available.
	const int PI_ORDER[13] = { ACT_DEVICE_0, ACT_DEVICE_1, ACT_DEVICE_2, ACT_DEVICE_3, ACT_DEVICE_4, ACT_DEVICE_5, ACT_DEVICE_6, ACT_DEVICE_7, ACT_DEVICE_8, ACT_DEVICE_9, ACT_DEVICE_10, ACT_DEVICE_11, ACT_DEVICE_12 };

	const int num_action_devices = 13;

	/**
	 * @brief Obtain the pin at a specific location in the reccomended AcT pin assignment based on Wiring Pi
	 *
	 * @param locator The index in the array
	 * @return int AcT device location
	 */
	int get_pi_location( int locator ) {
		return PI_ORDER[locator];
	}

	int get_action_tracer_device( int ACT_DEVICE_number ) {
		if( ACT_DEVICE_number > 13 && ACT_DEVICE_number < 0 ) {
			return -1;
		}

		switch( ACT_DEVICE_number ) {
			case 0:
				return ACT_DEVICE_0;
			case 1:
				return ACT_DEVICE_1;
			case 2:
				return ACT_DEVICE_2;
			case 3:
				return ACT_DEVICE_3;
			case 4:
				return ACT_DEVICE_4;
			case 5:
				return ACT_DEVICE_5;
			case 6:
				return ACT_DEVICE_6;
			case 7:
				return ACT_DEVICE_7;
			case 8:
				return ACT_DEVICE_8;
			case 9:
				return ACT_DEVICE_9;
			case 10:
				return ACT_DEVICE_10;
			case 11:
				return ACT_DEVICE_11;
			case 12:
				return ACT_DEVICE_12;
			default:
				return -1;
		}
	}

} // namespace ActionTracer

#endif