#include <GL/glut.h>
#include <cstdlib>
#include <time.h>
#include "tree.h"
#include "defines.h"
#include "skier.h"

using namespace std;

extern GLuint * textures;
extern Skier  * skier;

Tree::Tree(void) {

	x =  (float)(rand() % 800);
	y = -(float)(rand() % HILL_LENGTH);

} // end Tree()

void Tree::draw(void) {

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glPushMatrix();

	glTranslatef(x, y - skier->y, 0.0);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex2f( 0.0,  0.0);
	glTexCoord2f(0.0, 1.0);
	glVertex2f( 0.0, 64.0);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(32.0, 64.0);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(32.0,  0.0);
	glEnd();
	glPopMatrix();

} // end draw()

