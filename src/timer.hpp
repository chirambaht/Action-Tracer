#ifndef TIMER_HPP
#define TIMER_HPP
#include <sys/time.h>

class Timer {
  private:
	struct timeval start, finish;

  public:
	Timer() {
		// gettimeofday( &start, 0 );
	}

	~Timer() {}

	void reset() {
		gettimeofday( &start, 0 );
	}
	void tic() {
		gettimeofday( &start, 0 );
	}

	double toc_sec() {
		struct timeval end;
		gettimeofday( &end, 0 );
		return ( end.tv_sec - start.tv_sec ) + ( end.tv_usec - start.tv_usec ) / 1000000.0;
	}

	double toc_msec() {
		struct timeval end;
		gettimeofday( &end, 0 );
		return ( end.tv_sec - start.tv_sec ) * 1000.0 + ( end.tv_usec - start.tv_usec ) / 1000.0;
	}

	double toc_usec() {
		struct timeval end;
		gettimeofday( &end, 0 );
		return ( end.tv_sec - start.tv_sec ) * 1000000.0 + ( end.tv_usec - start.tv_usec );
	}
};

#endif