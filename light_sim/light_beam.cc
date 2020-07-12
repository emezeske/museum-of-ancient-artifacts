#include <GL/glut.h>
#include <cstdio>
#include "light_beam.h"

light_beam::light_beam( void ) :
	left_side( -10.0, 0.0, 0.0, 600.0 ),
	right_side( 0.0, -10.0, 300.0, 10.0 ),
	cur_state( FINITE_LEN )
{
} // end fnc light_beam()

bool light_beam::is_point_inside( const point_2d &p ) const 
{
		bool above_l = left_side.is_point_above( p ); 
		bool above_r = right_side.is_point_above( p ); 
		return ( above_l ^ above_r );
} // end fnc is_point_inside()

void light_beam::draw( void ) const 
{
	if ( cur_state == FINITE_LEN )
	{
		glColor3f( 1.0, 0.0, 0.0 );
		glPushMatrix();
		//glTranslatef( x, y, 0.0 );

		glBegin( GL_QUADS );
			glVertex2f( left_side.begin.x, 	left_side.begin.y);
			glVertex2f( left_side.vec.x, 	left_side.vec.y);
			glVertex2f( right_side.vec.x, 	right_side.vec.y);
			glVertex2f( right_side.begin.x,	right_side.begin.y);
		glEnd();
		glPopMatrix();
	}
	else std::printf( "Tried to draw an infinitely long light_beam!!\n" );
} // end fnc draw()
