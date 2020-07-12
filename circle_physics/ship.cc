#include "ship.h"

void Ship::draw()
{
	tex.bind();

	glPushMatrix();
		glTranslatef( x, y, 0.0 );
		glRotatef( theta, 0.0, 0.0, 1.0 );
		glBegin( GL_QUADS );
			glTexCoord2f( 0.0, 0.0 );
			glVertex2f( -r, -r );
			glTexCoord2f( 0.0, 1.0 );
			glVertex2f( -r, +r );
			glTexCoord2f( 1.0, 1.0 );
			glVertex2f( +r, +r );
			glTexCoord2f( 1.0, 0.0 );
			glVertex2f( +r, -r );
		glEnd();
	glPopMatrix();
}
