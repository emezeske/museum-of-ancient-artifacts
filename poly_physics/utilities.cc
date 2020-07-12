#include <cstdio>
#include <cstdlib>

#include "utilities.h"

namespace utilities
{

notify_level current_notify_level = FAULT;

notify_level set_notify_level( notify_level l )
{
	notify_level t = current_notify_level;
	current_notify_level = l;
	return t;
}

void die( int code )
{
	if ( code ) NOTIFY( INFO, "Exiting abnormally!" );
	else NOTIFY( INFO, "Exiting." );
	exit( code );
}

void _do_notify( notify_level l, const char *fmt, ... )
{
	va_list ap;
	va_start( ap, fmt );

	if ( l <= current_notify_level )
	{
		vfprintf( stderr, fmt, ap );
	}

	va_end( ap );
}

} // end namespace utilities
