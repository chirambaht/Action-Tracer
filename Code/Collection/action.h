#ifndef ACTION_H
#define ACTION_H

#include <string.h>

namespace ActionTracer {
	std::string headline_0 = "  ______               __      __                            ________                                               ";
	std::string headline_1 = " /      \\             /  |    /  |                          /        |                                              ";
	std::string headline_2 = "/AAAAAA  |  _______   tt |_   ii/   ______   _______        TTTTTTTT/______   ______    _______   ______    ______  ";
	std::string headline_3 = "AA |__AA | /       |/ tt   |  /  | /      \\ /       \\          TT | /      \\ /      \\  /       | /      \\  /      \\ ";
	std::string headline_4 = "AA    AA |/ccccccc/ tttttt/   ii |/oooooo  |nnnnnnn  |         TT |/rrrrrr  |aaaaaa  |/ccccccc/ /eeeeee  |/rrrrrr  |";
	std::string headline_5 = "AAAAAAAA |cc |        tt | __ ii |oo |  oo |nn |  nn |         TT |rr |  rr/ /    aa |cc |      ee    ee |rr |  rr/ ";
	std::string headline_6 = "AA |  AA |cc \\_____   tt |/  |ii |oo \\__oo |nn |  nn |         TT |rr |     /aaaaaaa |cc \\_____ eeeeeeee/ rr |      ";
	std::string headline_7 = "AA |  AA |cc       |  tt  tt/ ii |oo    oo/ nn |  nn |         TT |rr |     aa    aa |cc       |aa       |rr |      ";
	std::string headline_8 = "AA/   AA/  ccccccc/    tttt/  ii/  oooooo/  nn/   nn/          TT/ rr/       aaaaaaa/  ccccccc/  eeeeeee/ rr/       ";
	std::string headline_s = "                                                                                                                    ";

	std::string description = "Welcome to the (ACTISH) ACtion Tracer Interactive SHell. This can be used to ";

	int scan_i2c_for_tracers();

	void print_title();

	void save_to_file();

	void show_main_menu();
	void clear_screen();

	void silence_tracers();
	void silence_tracer( int pin );
} // namespace ActionTracer

#endif