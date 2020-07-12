#include <GL/glut.h>
#include <cstdio>
#include <cstdlib>
#include "light_beam.h"
#include "solid.h"

const int 	SCREEN_W = 800,
	  		SCREEN_H = 600;

void dissolve( const char *why ) 
{
	if ( why )
	{
		std::printf( "Exiting: %s\n", why );
		std::exit( -1 );
	} 
	else // Normal exit!
	{
		std::exit( 0 );
	}
} // end fnc dissolve()

void idle( void )
{
	static int lastTime;
	static int time = glutGet( GLUT_ELAPSED_TIME );
	static int delta = time - lastTime;
	lastTime = time;

	glutPostRedisplay();
} // end fnc idle()

void display( void ) 
{
	glClear( GL_COLOR_BUFFER_BIT );

	light_beam lb;
	lb.draw();

	point_2d p( 0.0, 0.0 );
	material m;
	m.color_R = 1.0;
	m.color_G = 0.0;
	m.color_B = 0.0;
	polygon ply;
	ply.vertices.push_back( point_2d( 0.0, 0.0 ) );
	ply.vertices.push_back( point_2d( 0.0, 30.0 ) );
	ply.vertices.push_back( point_2d( 30.0, 30.0 ) );
	ply.vertices.push_back( point_2d( 30.0, 0.0 ) );

	solid s( p, 0, m, ply);
	s.draw();
	int cnt = 0;
	for( polygon::vertices_iter i = ply.vertices.begin(); 
			i != ply.vertices.end(); ++i )
			if ( lb.is_point_inside( (*i) ) )
				++cnt;
	printf( "Found %d points inside the light_beam.\n", cnt );

	glutSwapBuffers();
} // end fnc display()

void keyboardDown( unsigned char key, int x, int y )
{
	switch( key ) {
		case 27:
			dissolve( 0 );
			break;
		case 'a':
			break;
		case 'd':
			break;
		case 'w':
			break;
	} // end switch(key)
} // end fnc keyboardDown()

void keyboardUp( unsigned char key, int x, int y ) 
{
	switch( key ) {
		case 'a':
			break;
		case 'd':
			break;
		case 'w':
			break;
	} // end switch(key)
} // end fnc keyboardUp()

void init_simulation( void ) 
{
} // end fnc init_simulation()

int main(int argc, char ** argv) {

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );
	//char gm_str[32];
	//snprintf( gm_str, 32, "%dx%d:32@60", SCREEN_W, SCREEN_H );
	//glutGameModeString( gm_str );
	//glutEnterGameMode();
	glutCreateWindow( "bounce" );
	glutReshapeWindow( SCREEN_W, SCREEN_H );

	glLoadIdentity();
	glutDisplayFunc( display );
	glutIdleFunc( idle );
	glutKeyboardFunc( keyboardDown );
	glutKeyboardUpFunc( keyboardUp );
	glOrtho( 0, SCREEN_W, 0, SCREEN_H, 0, 1.0 );
	glMatrixMode( GL_MODELVIEW );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );
	glClearColor( 1.0, 1.0, 1.0, 1.0 );

	init_simulation();

	glutMainLoop();

	return 0;
} // end main()
