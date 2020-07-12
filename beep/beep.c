#include <sys/io.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <map>
#include <string>

using std::map;
using std::string;

map< string, float > new_notes;

inline float freq_to_cyc( float f ) 
{
	const int clock_spd = 1190000;

	return clock_spd / f;
}

void init_notes()
{
	new_notes[ "a" ]  = freq_to_cyc( 440.000 );
	new_notes[ "a#" ] = freq_to_cyc( 466.164 );
	new_notes[ "b" ]  = freq_to_cyc( 493.883 );
	new_notes[ "b#" ] = freq_to_cyc( 523.251 );
	new_notes[ "c" ]  = freq_to_cyc( 523.251 );
	new_notes[ "c#" ] = freq_to_cyc( 554.365 );
	new_notes[ "d" ]  = freq_to_cyc( 587.330 );
	new_notes[ "d#" ] = freq_to_cyc( 622.254 );
	new_notes[ "e" ]  = freq_to_cyc( 659.255 );
	new_notes[ "e#" ] = freq_to_cyc( 698.456 );
	new_notes[ "f" ]  = freq_to_cyc( 698.456 );
	new_notes[ "f#" ] = freq_to_cyc( 739.989 );
	new_notes[ "g" ]  = freq_to_cyc( 783.991 );
	new_notes[ "g#" ] = freq_to_cyc( 830.609 );
}

inline int to_int( float f )
{
	return static_cast< int >( nearbyintf( f ) );
}

void play_note( string n, int octave, int dur_ms )
{
	float freq = new_notes[ n ];

	float scaling = octave >= 0 ? 1 / ( pow( octave + 1, 2 ) ) : pow( -( octave - 1 ), 2 );

	int adj_freq = to_int( freq * scaling );
	printf( "%s, %d, %f, %d\n", n.c_str(), octave, freq, adj_freq );

	outb( inb( 0x61 ) | 3, 0x61 );

	outb( 0xb6, 0x43 );

	outb( adj_freq & 0xff, 0x42 );

	outb( ( adj_freq >> 8 ) & 0xff, 0x42 );

	usleep( dur_ms * 1000 );

	outb( inb( 0x61 ) & 0xfc, 0x61 );
}

int main( int argc, char **argv )
{
	init_notes();

    ioperm( 0x61, 1, 1 );
    ioperm( 0x42, 2, 1 );

	for ( int i = -4; i < 4; ++i )
	{
		play_note( "a",	 i, 100 );
		play_note( "a#", i, 100 );
		play_note( "b",  i, 100 );
		play_note( "c",  i, 100 );
		play_note( "c#", i, 100 );
		play_note( "d",  i, 100 );
		play_note( "d#", i, 100 );
		play_note( "e",  i, 100 );
		play_note( "f",  i, 100 );
		play_note( "f#", i, 100 );
		play_note( "g",  i, 100 );
		play_note( "g#", i, 100 );
	}
}
