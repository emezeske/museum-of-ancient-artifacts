#include <GL/glut.h>
#include <stdlib.h>
#include <vector>

using std::vector;

#include "utilities.h"
#include "parser.h"
#include "world_obj.h"
#include "simulator.h"
#include "renderer.h"

const int
	SCREEN_W = 800,
	SCREEN_H = 600;

simulator sim;
renderer rndr;

void idle( void )
{
	static int
		last_update_time = glutGet( GLUT_ELAPSED_TIME ),
		last_render_time = glutGet( GLUT_ELAPSED_TIME ), 
		last_per_sec_time = glutGet( GLUT_ELAPSED_TIME ),  
		updates_per_sec = 0,
		renders_per_sec = 0;

	int
		cur_time = glutGet( GLUT_ELAPSED_TIME ),
		update_delta = cur_time - last_update_time,
		render_delta = cur_time - last_render_time;

	if ( update_delta > 0 )
	{
		last_update_time = cur_time;
		sim.update_all( static_cast< wu_t >( update_delta ) );
		++updates_per_sec;
	}

	if ( render_delta > 10 )
	{
		last_render_time = cur_time;
		glutPostRedisplay();
		++renders_per_sec;
	}

	if ( cur_time > last_per_sec_time + 1000 )
	{
		VNOTIFY( VERBOSE, "Simulation steps last second: %d", updates_per_sec );
		VNOTIFY( VERBOSE, "Frames last second: %d", renders_per_sec );
		updates_per_sec = renders_per_sec = 0;
		last_per_sec_time = cur_time;
	}
}

void display( void ) 
{
	glClear( GL_COLOR_BUFFER_BIT );

	rndr.draw_all();

	glutSwapBuffers();
}

void key_down( unsigned char k, int, int )
{
	switch ( k )
	{
		case 27:
			utilities::die( 0 );
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

bool world_obj_handler( const sect_parser::sect_info &i )
{
 	world_obj *o = new world_obj( i._contents );
	rndr.add_drawable( o );
	sim.add_physics_obj( o );

	return true;
}

void init_game()
{
	sect_parser::add_section_handler( "world_object", world_obj_handler );

	if ( !sect_parser::parse_file( "game_data/game_info.txt" ) ) utilities::die( -1 );
}

int main( int argc, char **argv ) 
{
	utilities::set_notify_level( VERBOSE );

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );

	glutCreateWindow( ">:)" );
	glutReshapeWindow( SCREEN_W, SCREEN_H );

	init_window();

	init_game();

	glutMainLoop();

	return 0;
}// end main()
