/*******************************************************************************
*
* Copyright 1997-2002 EnFlex Corp.
* All rights reserved.
* $Id: beep.c,v 1.5 2002/07/30 02:50:58 egrether Exp $
*
*******************************************************************************/

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

const int
	SCREEN_W = 512,
	SCREEN_H = 512;

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

void play_note( string n, int octave )
{
	float freq = new_notes[ n ];

	float scaling = octave >= 0 ? 1 / ( pow( octave + 1, 2 ) ) : pow( -( octave - 1 ), 2 );

	int adj_freq = to_int( freq * scaling );
	printf( "%s, %d, %f, %d\n", n.c_str(), octave, freq, adj_freq );

	outb( inb( 0x61 ) | 3, 0x61 );

	outb( 0xb6, 0x43 );

	outb( adj_freq & 0xff, 0x42 );

	outb( ( adj_freq >> 8 ) & 0xff, 0x42 );
}

void stop_note()
{
	outb( inb( 0x61 ) & 0xfc, 0x61 );
}

void key_down( unsigned char key, int, int )
{
	switch( key )
	{
		case "a": play_note( "c", 1, 100 );
		case "s": play_note( "d", 1, 100 );
		case "d": play_note( "e", 1, 100 );
		case "f": play_note( "f", 1, 100 );
		case "g": play_note( "g", 1, 100 );
		case "h": play_note( "a", 2, 100 );
		case "j": play_note( "b", 2, 100 );
		case "k": play_note( "c", 2, 100 );
		case "l": play_note( "d", 2, 100 );
		case ";": play_note( "e", 2, 100 );
		case "'": play_note( "f", 2, 100 );
		case "s": play_note( "g", 2, 100 );
	}
}

void key_up( unsigned char, int, int )
{
	stop_note();
}

void display( void ) 
{
	glClear( GL_COLOR_BUFFER_BIT );

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
	glutIdleFunc( idle );

  	glMatrixMode( GL_PROJECTION );
  	glLoadIdentity();
	glOrtho( -SCREEN_W / 2, SCREEN_W / 2, -SCREEN_H / 2, SCREEN_H / 2, 0, 1.0 );

  	glMatrixMode( GL_MODELVIEW );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );
	glEnable( GL_TEXTURE_2D );
	glutSetCursor( GLUT_CURSOR_NONE );
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

	glutCreateWindow( "synth" );
	glutReshapeWindow( SCREEN_W, SCREEN_H );

	init_window();

	glutMainLoop();

}
