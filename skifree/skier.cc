#include <GL/glut.h>
#include <math.h>
#include "skier.h"
#include "defines.h"

extern GLuint * textures;

Skier::Skier(void) {

	x =  384;
	y = -300;
	yVel = 0;
	movingLeft = movingRight = false;

}  // end Skier()

void Skier::update(int delta) {

	if(movingLeft)
		x += (yVel * tan(PI / 10)) * delta;
	else if(movingRight)
		x -= (yVel * tan(PI / 10)) * delta;

	yVel += GRAV_ACC_RATE * delta;

	if(slowingDown)
		yVel -= SKIER_DEC_RATE * delta;

	y += yVel * delta;

	if(yVel < MAX_SKIER_SPEED)
		yVel = MAX_SKIER_SPEED;
	else if(yVel > 0)
		yVel = 0;

} // end update()

void Skier::draw(void) {

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glPushMatrix();

	glTranslatef(x, 300.0, 0.0);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex2f( 0.0,  0.0);
	glTexCoord2f(0.0, 1.0);
	glVertex2f( 0.0, 32.0);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(32.0, 32.0);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(32.0,  0.0);
	glEnd();
	glPopMatrix();

} // end draw()
