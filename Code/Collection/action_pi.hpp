/**
 * This is used to define PI pins for the Action Tracer Suite of Applications.
 */
#ifndef ACTION_PI_H
#define ACTION_PI_H

/*
 +-----+-----+---------+------+--Pi Zero-+------+---------+-----+-----+
 | AcT | wPi |   Name  | Mode | Physical | Mode | Name    | wPi | AcT |
 +-----+-----+---------+------+----++----+------+---------+-----+-----+
 |     |     |   3V3   |      |  1 || 2  |      |   5v    |     |     |
 |   2 |   8 |  Data   |      |  3 || 4  |      |   5v    |     |     |
 |   3 |   9 |  Clock  |      |  5 || 6  |      |   GND   |     |     |
 |   4 |   7 |         |      |  7 || 8  |      |         | 15  | 14  |
 |     |     |   GND   |      |  9 || 10 |      |         | 16  | 15  |
 |  17 |   0 |         |      | 11 || 12 |      |         | 1   | 18  |
 |  27 |   2 |         |      | 13 || 14 |      |   GND   |     |     |
 |  22 |   3 |         |      | 15 || 16 |      |         | 4   | 23  |
 |     |     |   3V3   |      | 17 || 18 |      |         | 5   | 24  |
 |  10 |  12 |         |      | 19 || 20 |      |   GND   |     |     |
 |   9 |  13 |         |      | 21 || 22 |      |         | 6   | 25  |
 |  11 |  14 |         |      | 23 || 24 |      |         | 10  | 8   |
 |     |     |   GND   |      | 25 || 26 |      |         | 11  | 7   |
 |   0 |  30 |         |      | 27 || 28 |      |         | 31  | 1   |
 |   5 |  21 |         |      | 29 || 30 |      |   GND   |     |     |
 |   6 |  22 |         |      | 31 || 32 |      |         | 26  | 12  |
 |  13 |  23 |         |      | 33 || 34 |      |   GND   |     |     |
 |  19 |  24 |         |      | 35 || 36 |      |         | 27  | 16  |
 |  26 |  25 |         |      | 37 || 38 |      |         | 28  | 20  |
 |     |     |   GND   |      | 39 || 40 |      |         | 29  | 21  |
 +-----+-----+---------+------+----++----+------+---------+-----+-----+
 | AcT | wPi |   Name  | Mode | Physical | Mode | Name    | wPi | AcT |
 +-----+-----+---------+------+-Pi Zero--+------+---------+-----+-----+
*/

namespace ActionTracer {

#define ACT_I2C_DATA  8
#define ACT_I2C_CLOCK 9

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

	// An array with all the Pi pins available.
	int PI_ORDER[12] = { ACT_DEVICE_1, ACT_DEVICE_2, ACT_DEVICE_3, ACT_DEVICE_4, ACT_DEVICE_5, ACT_DEVICE_6, ACT_DEVICE_7, ACT_DEVICE_8, ACT_DEVICE_9, ACT_DEVICE_10, ACT_DEVICE_11, ACT_DEVICE_12 };

	/**
	 * @brief Obtain the pin at a specific location in the reccomended AcT pin assignment based on Wiring Pi
	 *
	 * @param locator The index in the array
	 * @return int AcT device location
	 */
	int ActionTracer::get_pi_location( int locator ) {
		return PI_ORDER[locator];
	}

} // namespace ActionTracer

#endif