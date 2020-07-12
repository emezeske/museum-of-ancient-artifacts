#include <GL/glut.h>
#include <stdlib.h>
#include "defines.h"
#include "camera.h"
#include "worldobject.h"
#include "mobileworldobject.h"
#include "hero.h"
#include "fixedworldobject.h"

extern GLuint * textures; 
extern Hero   * hero;
extern Camera * camera;

// default constructor
FixedWorldObject::FixedWorldObject() {
	x      = 0;
	y      = 0;
	width  = 0;
	height = 0;
	textureNumber = 0;
} // end FixedWorldObject()

// constructor which takes a pointer to a different FixedWorldObject
FixedWorldObject::FixedWorldObject(FixedWorldObject * tempFWO) {
	x      = 0;
	y      = 0;
	width  = tempFWO->width;
	height = tempFWO->height;
	textureNumber = tempFWO->textureNumber;
} // end FixedWorldObject()

void FixedWorldObject::draw(void) {
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	glTranslatef(x - camera->x, y - camera->y, 0.0);
	glBindTexture(GL_TEXTURE_2D, textures[textureNumber + NUM_HERO_FRAMES]);

	glBegin(GL_QUADS);
  	glTexCoord2f(  0.0,   0.0);
  	glVertex2f(    0.0,   0.0);	 
  	glTexCoord2f(  0.0,   1.0);
  	glVertex2f(    0.0,   height);	 
  	glTexCoord2f(  1.0,   1.0);
  	glVertex2f(  width,   height);	 
  	glTexCoord2f(  1.0,   0.0);
  	glVertex2f(  width,   0.0);	 
  	glEnd();
 
	glPopMatrix();	
	glDisable(GL_TEXTURE_2D);	
} // end draw()
