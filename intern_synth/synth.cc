#include <GL/glut.h>
#include <sys/io.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <map>
#include <string>

using std::map;
using std::string;

struct note
{
	string nt;
	int octave;

	note() : nt( "" ), octave( -99 ) {}
	note( string n, int o ) : nt( n ), octave( o ) {}

	bool operator == ( const note &rhs )
	{
		return ( nt == rhs.nt && octave == rhs.octave );
	}
};

bool note_playing;
note current_note;

typedef map< char, note > key_to_note;
typedef map< char, note >::iterator key_to_note_iter;

key_to_note keybd_map;

const int
	SCREEN_W = 1000,
	SCREEN_H = 64,
	DEADBAND = 50,
	MIDDLE_LOW = SCREEN_W / 2 - DEADBAND / 2,
	MIDDLE_HIGH = SCREEN_W / 2 + DEADBAND / 2;

float bend = 1.0;

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

	keybd_map[ 'a' ] = note( "c", 0 );
	keybd_map[ 's' ] = note( "d", 0 );
	keybd_map[ 'd' ] = note( "e", 0 );
	keybd_map[ 'f' ] = note( "f", 0 );
	keybd_map[ 'g' ] = note( "g", 0 );
	keybd_map[ 'h' ] = note( "a", 1 );
	keybd_map[ 'j' ] = note( "b", 1 );
	keybd_map[ 'k' ] = note( "c", 1 );
	keybd_map[ 'l' ] = note( "d", 1 );
	keybd_map[ ';' ] = note( "e", 1 );
	keybd_map[ '\'' ] = note( "f", 1 );
	keybd_map[ 'q' ] = note( "c#", 0 );
	keybd_map[ 'w' ] = note( "d#", 0 );
	keybd_map[ 'r' ] = note( "f#", 0 );
	keybd_map[ 't' ] = note( "g#", 0 );
	keybd_map[ 'y' ] = note( "a#", 1 );
	keybd_map[ 'i' ] = note( "c#", 1 );
	keybd_map[ 'o' ] = note( "d#", 1 );
	keybd_map[ '[' ] = note( "f#", 1 );
}

inline int to_int( float f )
{
	return static_cast< int >( nearbyintf( f ) );
}

void play_note( note the_note )
{
	current_note = the_note;
	note_playing = true;

	float freq = new_notes[ the_note.nt ];

	int octave = the_note.octave;

	float scaling = octave >= 0 ? 1 / ( pow( 2, octave + 1 ) ) : pow( 2, -( octave - 1 ) );

	scaling *= bend;

	int adj_freq = to_int( freq * scaling );

	outb( inb( 0x61 ) | 3, 0x61 );

	outb( 0xb6, 0x43 );

	outb( adj_freq & 0xff, 0x42 );

	outb( ( adj_freq >> 8 ) & 0xff, 0x42 );
}

void stop_note()
{
	note_playing = false;
	outb( inb( 0x61 ) & 0xfc, 0x61 );
}

void key_down( unsigned char key, int, int )
{
	int modifiers = glutGetModifiers();

	if ( modifiers & GLUT_ACTIVE_CTRL ) key += 96;

	switch ( key )
	{
		case ' ': stop_note(); break;

		case 27: exit( 0 ); 
	
		default:

			bool found = false; 
			
			for ( key_to_note_iter i = keybd_map.begin(); i != keybd_map.end() && !found; ++i )
			{
				if ( i->first == key ) found = true;
			}
			
			if ( found )
			{
				note next_note = keybd_map[ key ];

				if ( modifiers & GLUT_ACTIVE_CTRL ) next_note.octave += 1;

				play_note( next_note );
			}
	}
}

void key_up( unsigned char key, int, int )
{
	bool found = false;

	for ( key_to_note_iter i = keybd_map.begin(); i != keybd_map.end() && !found; ++i )
	{
		if ( i->first == key ) found = true;
	}

	if ( found )
	{
		note note_up = keybd_map[ key ];
		
		if ( note_playing && current_note == note_up ) stop_note();
	}
}

void mouse_move( int x, int )
{
	const float scaling_factor = 1.0;

	float new_bend = 1.0;

	if ( x < MIDDLE_LOW )
	{
		new_bend += scaling_factor * ( static_cast< float >( MIDDLE_LOW - x ) / MIDDLE_LOW );
	}
	if ( x > MIDDLE_HIGH )
	{
		new_bend -= scaling_factor * static_cast< float >( x - MIDDLE_LOW - DEADBAND ) / MIDDLE_LOW;
	}

	if ( new_bend != bend )
	{
		bend = new_bend;
		play_note( current_note );
	}
}

void display( void ) 
{
	glClear( GL_COLOR_BUFFER_BIT );

	glBegin( GL_QUADS );

	glColor3f( 0.0, 0.0, 0.1 );

	glVertex2f( 0, 0 );
	glVertex2f( 0, SCREEN_H );

	glColor3f( 0.0, 0.0, 1.0 );

	glVertex2f( MIDDLE_LOW, SCREEN_H );
	glVertex2f( MIDDLE_LOW, 0 );

	glColor3f( 0.1, 0.9, 0.1 );

	glVertex2f( MIDDLE_LOW, 0 );
	glVertex2f( MIDDLE_LOW, SCREEN_H );
	glVertex2f( MIDDLE_HIGH, SCREEN_H );
	glVertex2f( MIDDLE_HIGH, 0 );

	glColor3f( 1.0, 0.0, 0.0 );

	glVertex2f( MIDDLE_HIGH, 0 );
	glVertex2f( MIDDLE_HIGH, SCREEN_H );

	glColor3f( 0.1, 0.0, 0.0 );

	glVertex2f( SCREEN_W, SCREEN_H );
	glVertex2f( SCREEN_W, 0 );

	glEnd();

	glutSwapBuffers();
}

void idle( void )
{
	glutPostRedisplay();
}

void init_window( void )
{
	glutIgnoreKeyRepeat( 1 );
	
	glutDisplayFunc( display );
	glutKeyboardFunc( key_down );
	glutKeyboardUpFunc( key_up );
	glutPassiveMotionFunc( mouse_move );
	glutIdleFunc( idle );

  	glMatrixMode( GL_PROJECTION );
  	glLoadIdentity();
	glOrtho( 0, SCREEN_W, 0, SCREEN_H, 0, 1.0 );

  	glMatrixMode( GL_MODELVIEW );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );
	glEnable( GL_TEXTURE_2D );
	glClearColor( 0.1, 0.1, 0.1, 1.0 );

	glutSwapBuffers();
} 

int main( int argc, char **argv )
{
	init_notes();

    ioperm( 0x61, 1, 1 );
    ioperm( 0x42, 2, 1 );

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );

	glutCreateWindow( "|'|'| |'|'|'| |" );
	glutReshapeWindow( SCREEN_W, SCREEN_H );

	init_window();

	glutMainLoop();
}
