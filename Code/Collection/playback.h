#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <string>

namespace ActionTracer {
	class ActionPlayer {
	  private:
		void _correct_file_data();
		void _get_files_in_directory();

		std::string filename;

	  public:
		ActionPlayer();
		~ActionPlayer();

		void open_file();
		void play_file();
	};
} // namespace ActionTracer

#endif