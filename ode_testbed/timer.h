#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <sys/time.h>
#include <time.h>

class Timer
{
	private:
		timeval
			last_tv, 
			mostRecent_tv;

	public:
		Timer()
		{
			gettimeofday( &last_tv, NULL );
		}

		void reset()
		{
			last_tv.tv_sec = mostRecent_tv.tv_sec;
			last_tv.tv_usec = mostRecent_tv.tv_usec;
		}

		unsigned long getElapsedTime()
		{
			gettimeofday( &mostRecent_tv, NULL );

			time_t elapsedSec = mostRecent_tv.tv_sec - last_tv.tv_sec;
			suseconds_t elapsedUSec = mostRecent_tv.tv_usec - last_tv.tv_usec;

			return 1000000 * elapsedSec + elapsedUSec;
		}
};

#endif // __UTILITIES_H__
