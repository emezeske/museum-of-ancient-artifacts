#include <GL/glut.h>
#include "defines.h"
#include "worldobject.h"
#include "fixedworldobject.h"
#include "viewport.h"

extern GLuint   * textures; 
extern Viewport viewport;

// default constructor
FixedWorldObject::FixedWorldObject() {

	x      = viewport.x + SCREEN_WIDTH  / 2;
	y      = viewport.y + SCREEN_HEIGHT / 2;
	width  = 0;
	height = 0;
	textureNumber = 0;

} // end FixedWorldObject()

// constructor which takes a pointer to a different FixedWorldObject
FixedWorldObject::FixedWorldObject(FixedWorldObject * tempFWO) {

	x      = tempFWO->x;
	y      = tempFWO->y;
	width  = tempFWO->width;
	height = tempFWO->height;
	textureNumber = tempFWO->textureNumber;

} // end FixedWorldObject()

// constructor takes pointers to an old FWO and a menu item 
FixedWorldObject::FixedWorldObject(FixedWorldObject * menuItem, FixedWorldObject * oldFWO) {

	x      = oldFWO->x;
	y      = oldFWO->y;
	width  = menuItem->width;
	height = menuItem->height;
	textureNumber = menuItem->textureNumber;

} // end FixedWorldObject()

void FixedWorldObject::draw(bool tempCurFWO) {

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	glTranslatef(x - viewport.x, y - viewport.y, 0.0);
	glBindTexture(GL_TEXTURE_2D, textures[textureNumber]);

	if(tempCurFWO)
		glColor4f(1.0, 1.0, 1.0, 1.0);
	else
		glColor4f(0.7, 0.7 , 0.7, 1.0);

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

// this function draws the object menu tiles
void FixedWorldObject::drawMenuTile(int colNum, int rowNum, bool selected) {

	float tileSize = 100;

	glPushMatrix();

	glTranslatef(colNum * tileSize, rowNum * tileSize, 0.0);
	glBindTexture(GL_TEXTURE_2D, textures[textureNumber]);

	// highlight selected item
	glEnable(GL_TEXTURE_2D);

	if(selected)
		glColor4f(1.0, 1.0, 1.0, 1.0);
	else
		glColor4f(0.6, 0.6, 0.6, 1.0);

	glBegin(GL_QUADS);
  	glTexCoord2f(       0.0,   0.0);
  	glVertex2f(         5.0,   5.0);	 
  	glTexCoord2f(       0.0,   1.0);
  	glVertex2f(         5.0,   tileSize-5);	 
  	glTexCoord2f(       1.0,   1.0);
  	glVertex2f(  tileSize-5,   tileSize-5);	 
  	glTexCoord2f(       1.0,   0.0);
  	glVertex2f(  tileSize-5,   5.0);	 
  	glEnd();
	
	glPopMatrix();	
	glDisable(GL_TEXTURE_2D);	

} // end drawMenuTile()
