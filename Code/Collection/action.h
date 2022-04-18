#ifndef ACTION_H
#define ACTION_H

#include <string>

namespace ActionTracer {

	std::string headline_1 = "         db          w   w                88888                                ";
	std::string headline_2 = "        dPYb   .d8b w8ww w .d8b. 8d8b.      8   8d8b .d88 .d8b .d88b 8d8b      ";
	std::string headline_3 = "       dPwwYb  8     8   8 8' .8 8P Y8      8   8P   8  8 8    8.dP' 8P        ";
	std::string headline_4 = "      dP    Yb `Y8P  Y8P 8 `Y8P' 8   8      8   8    `Y88 `Y8P `Y88P 8         ";
	std::string headline_s = "                                                                               ";

	std::string description = "Welcome to the (ACTISH) ACtion Tracer Interactive SHell. This can be used to configure the devices before use and generate functional setup files.";

	std::string live_map_l = "    + -------------------------------------------------------------- +    ";
	std::string live_map_d = "    |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 | 9  | 10 | 11 | 12 |    ";

	size_t current_detected_devices = 0;

	int scan_i2c_for_tracers();

	void print_title();
	void print_status();

	void save_to_file();

	void show_main_menu();
	void clear_screen();

	int check_device( uint8_t ACT_device );

	void silence_tracers();
	void silence_tracer( int pin );
} // namespace ActionTracer

#endif