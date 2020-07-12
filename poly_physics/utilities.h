#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdarg.h>

#define str( s ) #s
#define xstr( s ) str( s )

#define NOTIFY( l, s ) utilities::_do_notify( l, __FILE__ ":" xstr( __LINE__ ) ":%s: " s "\n", __func__ )
#define VNOTIFY( l, s, ... ) utilities::_do_notify( l, __FILE__ ":" xstr( __LINE__ ) ":%s: " s "\n", __func__, __VA_ARGS__ )

enum notify_level
{
	OFF = 0,
	FAULT,
	INFO,
	VERBOSE
};

namespace utilities
{

void _do_notify( notify_level l, const char *fmt, ... );

template< class T >
void free_contained( T &c )
{
	for ( typename T::iterator i = c.begin(); i != c.end(); ++i )
		delete ( *i );
}

notify_level set_notify_level( notify_level l );

void die( int code );

} // end namespace utilities

#endif // UTILITIES_H
