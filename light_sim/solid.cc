#include <GL/glut.h>
#include "solid.h"

void solid::draw( void )
{
	glColor3f( mat.color_R, mat.color_G, mat.color_B );
	glPushMatrix();
	glTranslatef( pos.x, pos.y, 0.0F );

	glBegin( GL_POLYGON );
		for( polygon::vertices_iter i = bounds.vertices.begin(); i != bounds.vertices.end(); ++i )
			glVertex2f( (*i).x, (*i).y);
	glEnd();

	glPopMatrix();
}; // end fnc draw()
