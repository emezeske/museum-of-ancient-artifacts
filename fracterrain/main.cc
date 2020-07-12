#include "fracterrain.h"

#include <time.h>
#include <cstdlib>
#include <math.h>
#include <GL/glut.h>

int screen_w = 800,
	screen_h = 600;

Fracterrain *terrain;

void key_down( unsigned char key, int x, int y )
{
	switch ( key )
	{
		case 27:
			std::exit( 0 );
		default:
			break;
	}
} // end fnc keyDown()

float r = 0.0;

void idle( void )
{
	static int lastTime = glutGet( GLUT_ELAPSED_TIME );
	int time = glutGet( GLUT_ELAPSED_TIME );
	int delta = time - lastTime;
	lastTime = time;

	r += 0.02 * delta;

	glutPostRedisplay();
} // end fnc idle()

void draw( void ) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotatef( r, 0.0, 0.0, 1.0 );

	terrain->draw();

	glPopMatrix();
	glutSwapBuffers();
} // end fnc draw()

void initGL( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

	glutCreateWindow( "Shig" );
	glutReshapeWindow( screen_w, screen_h );
 
	glutIgnoreKeyRepeat( 0 );
	
	glutDisplayFunc( draw );
	glutIdleFunc( idle );
	glutKeyboardFunc( key_down );
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45, (GLdouble)screen_w/(GLdouble)screen_h, 1, 100 );
	gluLookAt( 0.0, -20.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
	glViewport( 0, 0, screen_w, screen_h );
	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
} // end fnc initGL()

int main( int argc, char **argv )
{
	terrain = new Fracterrain( time( NULL ), 10, 0.54 );
	initGL( argc, argv );
	glutMainLoop();
} // end fnc main()
