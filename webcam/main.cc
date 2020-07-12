#include <GL/glut.h>
#include <stdio.h> 
#include <cmath>

#include "utilities.h"
#include "globals.h"
#include "edge_detector.h"
#include "camera.h"

camera
	*cam_l = 0,
	*cam_r = 0;

unsigned char
	*image_l,
	*image_r;

float threshold = 0.03;

edge_detector edge_det( threshold );

void idle( void )
{
	image_l = cam_l->next_frame();
	image_r = cam_r->next_frame();

	const unsigned
		mid_x = CAM_W / 2,
		mid_y = CAM_H / 2;

	edge_det.detect( image_l, mid_x, mid_y );
	edge_det.draw_debug( image_l );

	edge_det.detect( image_r, mid_x, mid_y );
	edge_det.draw_debug( image_r );

	glutPostRedisplay();
}

void display( void ) 
{
	glClear( GL_COLOR_BUFFER_BIT );

	if ( cam_l && image_l && cam_r && image_r )
	{
		glRasterPos2i( 0 , 0 );
		glDrawPixels( CAM_W , CAM_H , GL_BGR , GL_UNSIGNED_BYTE , image_l );
		glRasterPos2i( CAM_W , 0 );
		glDrawPixels( CAM_W , CAM_H , GL_BGR , GL_UNSIGNED_BYTE , image_r );
		glFlush();
	}
	else NOTIFY( FAULT, "Frame capture failed!" );

	glutSwapBuffers();
}

void key_down( unsigned char k, int, int )
{
	switch ( k )
	{
		case 27:

			utilities::die( 0 );
			break;

		case '+':

			threshold += 0.001;
			edge_det.set_threshold( threshold );
			break;

		case '-':

			threshold -= 0.001;
			edge_det.set_threshold( threshold );
			break;
	}
}

void key_up( unsigned char, int, int )
{
}

void init_window( void )
{
	glutIgnoreKeyRepeat( 1 );
	
	glutDisplayFunc( display );
	glutKeyboardFunc( key_down );
	glutKeyboardUpFunc( key_up );
	glutIdleFunc( idle );

	glOrtho( 0, SCREEN_W, 0, SCREEN_H, -1.0, 1.0 );

	glutSetCursor( GLUT_CURSOR_NONE );
	glClearColor( 0.1, 0.1, 0.9, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT );
	glutSwapBuffers();
	glFlush();
} 

camera *init_camera( const char *dev, int channel )
{
	camera
		*retval = 0,
		*cam = 0;

	try
	{
		cam = new camera( dev, channel );
	}
	catch ( camera_exception ex )
	{
		VNOTIFY( FAULT, "Opening of camera failed: %s", ex.what() );
	}

	if ( cam != 0 )
	{
		if ( cam->set_size( CAM_W, CAM_H ) )
		{
			if ( cam->set_24_bit_color() )
			{
				if ( cam->init_framebuffer( CAPTURE_VIA_READ ) )
				{
					retval = cam;
				}
				else NOTIFY( FAULT, "Initializing framebuffer failed." );
			}
			else NOTIFY( FAULT, "Setting camera color depth failed." );
		}
		else NOTIFY( FAULT, "Setting camera size failed." );
	}

	return retval;
}

int main( int argc, char **argv ) 
{
	utilities::set_notify_level( VERBOSE );

	if ( ( cam_l = init_camera( "/dev/video0", 0 ) ) == 0 ) utilities::die( -1 );
	if ( ( cam_r = init_camera( "/dev/video1", 0 ) ) == 0 ) utilities::die( -1 );

 	image_l = cam_l->next_frame();
 	image_r = cam_r->next_frame();

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );

	glutCreateWindow( "camtastic!" );
	glutReshapeWindow( SCREEN_W, SCREEN_H );

	init_window();

	glutMainLoop();

	return 0;
}
