#include <GL/glut.h>
#include <float.h>
#include "utilities.h"
#include "ltga.h"
#include "ball.h"
#include <math.h>
#include <assert.h>
#include <stdlib.h>

const int NUM_BALLS = 12;  // Justin Davis
Ball *balls[NUM_BALLS];

const int
	SCREEN_W = 800,
	SCREEN_H = 600;

void idle( void ) {
	static Timer tmr;
	unsigned long elapsedUS = tmr.getElapsedTime();
	PredictedCollision lastCol( 0, 0, 0 );

	if ( elapsedUS != 0 )
	{
		tmr.reset();
		double delta = elapsedUS * 0.001;

		while ( 1 )
		{
			bool collisionOccurred = false;
			PredictedCollision col( 0, 0, DBL_MAX );

			for ( int i = 0; i < NUM_BALLS; ++i )
			{
				for ( int z = i + 1; z < NUM_BALLS; ++z )
				{
					double thisTime = balls[i]->timeOfCollision( *balls[z], delta );
					if ( thisTime != CollisionObj::NO_COLLISION && thisTime < col.timeOfCollision )
					{
						if ( static_cast< CollisionObj *>( balls[i] ) != lastCol.c1 && static_cast< CollisionObj *>( balls[z] ) != lastCol.c2 )
						{
							if ( thisTime >= 0.0000000000001 )
							{
								collisionOccurred = true;
								col = PredictedCollision( balls[i], balls[z], thisTime );
							}
							else printf( "Funktastic\n\n" );
						}
					}
				}
			}

			if ( collisionOccurred )
			{
				lastCol = col;

				for ( int i = 0; i < NUM_BALLS; ++i )
					balls[i]->move( col.timeOfCollision );

				col.doCollision();
				delta -= col.timeOfCollision;
			}
			else
			{
				for ( int i = 0; i < NUM_BALLS; ++i )
					balls[i]->move( delta );
				break;
			}
		}
	}
	
	glutPostRedisplay();
} // end idle()

void display( void ) {
	glClear( GL_COLOR_BUFFER_BIT );

	for ( int i = 0; i < NUM_BALLS; ++i )
		balls[i]->draw();
	
	glutSwapBuffers();
} // end display()

void keyDown( unsigned char k, int, int )
{
	switch ( k )
	{
		case 27:
			exit( 0 );
			break;
		case 'j':
			printf( "Joe is slightly gay.\n" );
			break;
		default:
			break;
	}
} // end keyDown()

void keyUp( unsigned char, int, int )
{
} // end keyUp()

void initWindow( void ) {
	glutIgnoreKeyRepeat( 1 );
	
	glutDisplayFunc( display );
	glutKeyboardFunc( keyDown );
	glutKeyboardUpFunc( keyUp );
	glutIdleFunc( idle );

  	glMatrixMode( GL_PROJECTION );
  	glLoadIdentity();
	glOrtho( -SCREEN_W / 2, SCREEN_W / 2, -SCREEN_H / 2, SCREEN_H / 2, 0, 1.0 );
  	glMatrixMode( GL_MODELVIEW );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );
	glEnable( GL_TEXTURE_2D );
	glutSetCursor( GLUT_CURSOR_NONE );
	glClearColor( 0.1, 0.0, 0.84, 1.0 );

	glutSwapBuffers();
} // end initWindow()

int main( int argc, char **argv ) {
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );

	glutCreateWindow( "wacky" );
	glutReshapeWindow( SCREEN_W, SCREEN_H );
	//glutGameModeString( "1024x768:32@60" );
	//glutEnterGameMode();

	initWindow();

	balls[0] = new Ball( -128.0, 0.0, +0.05, 0.0, 15.0, 20.0, "gfx/balltex.tga" );
	balls[1] = new Ball(    0.0, 0.0,  0.00, 0.0, 15.0, 20.0, "gfx/balltex.tga" );
	balls[2] = new Ball( +128.0, 0.0, -0.05, 0.0, 15.0, 20.0, "gfx/balltex.tga" );
	balls[3] = new Ball( -64.0, -128.0, 0.00, 0.06, 18.0, 30.0, "gfx/balltex.tga" );
	balls[4] = new Ball( -64.0, +128.0, 0.08, 0.08, 20.0, 40.0, "gfx/balltex.tga" );
	balls[5] = new Ball( -200.0, -200.0, -0.00, 0.0, 10.0, 9.0, "gfx/balltex.tga" );
	balls[6] = new Ball( -128.0, +64.0, -0.08, 0.8, 12.0, 20.0, "gfx/balltex.tga" );
	balls[7] = new Ball( +128.0, -64.0, -0.02, 0.10, 14.0, 23.0, "gfx/balltex.tga" );
	balls[8] = new Ball( +128.0, +64.0, -0.08, 0.12, 16.0, 29.0, "gfx/balltex.tga" );
	balls[9] = new Ball( -64.0, -64.0, -0.00, 0.6, 18.0, 33.0, "gfx/balltex.tga" );
	balls[10] = new Ball( -64.0, 64.0, -0.00, 0.6, 18.0, 33.0, "gfx/balltex.tga" );
	balls[11] = new Ball( +200.0, +200.0, 0.00, 0.0, 50.0, 300.0, "gfx/balltex.tga" );

	glutMainLoop();
	return 0;
}// end main()
