#include "playback.h"

#include <dirent.h>

using namespace ActionTracer;

ActionTracer::ActionPlayer::ActionPlayer( /* args */ ) {
}

ActionTracer::ActionPlayer::~ActionPlayer() {
}

void ActionTracer::ActionPlayer::_get_files_in_directory() {
	if( auto dir = opendir( "." ) ) {
		while( auto f = readdir( dir ) ) {
			if( !f->d_name || f->d_name[0] == '.' )
				continue; // Skip everything that starts with a dot

			printf( "File: %s\n", f->d_name );
		}
		closedir( dir );
	}
}