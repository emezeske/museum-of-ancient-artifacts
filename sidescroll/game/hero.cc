#include <GL/glut.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "camera.h"
#include "worldobject.h"
#include "mobileworldobject.h"
#include "fixedworldobject.h"
#include "hero.h"
#include "defines.h"

extern GLuint 		* textures;
extern FixedWorldObject * fixedWorldObject;
extern Camera 		* camera;
extern bool 	       ** collisionBlocks;

extern int LEVEL_WIDTH, LEVEL_HEIGHT, COL_BLOCK_SIZE;

// constructor
Hero::Hero(float initX    = 0, float initY    = 0,
           float initVelX = 0, float initVelY = 0 ) {

	x    = initX; 
	y    = initY;
	velX = initVelX;
	velY = initVelY;

	jumping = movingLeft = movingRight = false;

	currentFrame = frameTimeCounter = 0;
	frameDelta   = 1;

	width  = HERO_WIDTH;
	height = HERO_HEIGHT;

} // end Hero::Hero()

void Hero::updateState(int delta) {

	frameTimeCounter += delta;

	// update the hero's frame counter
	if(movingLeft || movingRight) {
		if(frameTimeCounter >= HERO_FRAME_TIME) {
			if(currentFrame >= NUM_HERO_FRAMES)
				frameDelta = -1;
			if(currentFrame == 0)
				frameDelta =  1;
			frameTimeCounter = 0;
			currentFrame += frameDelta;
		} // end if
	} else 
		currentFrame = frameTimeCounter = 0;

	float lastX = x, lastY = y;

	velX = 0.0;
	if(movingLeft)  velX = -0.2;
	if(movingRight) velX =  0.2;

	velY -= GRAVITY_STRENGTH * delta;

	x += velX * delta;
	y += velY * delta;

	checkCollisions(lastX, lastY, delta);


} // end Hero::updateState()

void Hero::jump(void) {

	if(!jumping) {
		velY = HERO_JUMP_POWER;
		jumping = 1;
	}

} // end Hero::jump()

void Hero::checkCollisions(float lastX, float lastY, int delta) {

	// crappy collision checking for right now  
	if(x < 0) { x = 0; velX = 0;}
	if(y < 0) { y = 0; jumping = 0; velY = 0;}

	if(y > LEVEL_HEIGHT - HERO_HEIGHT){
		y = LEVEL_HEIGHT - HERO_HEIGHT;
		velY = 0;
		jumping = 0;
	} // end if
	if(x > LEVEL_WIDTH - HERO_WIDTH){
		x = LEVEL_WIDTH - HERO_WIDTH;
		velX = 0;
	} // end if
	
	int heroColX      = (int)floor(x / COL_BLOCK_SIZE);
	int heroColY      = (int)floor(y / COL_BLOCK_SIZE);
	int heroColWidth  = (int)floor(HERO_WIDTH / COL_BLOCK_SIZE);
	int heroColHeight = (int)floor(HERO_HEIGHT / COL_BLOCK_SIZE);
	
	for(int i = 0; i < heroColWidth; ++i)
		for(int z = 0; z < heroColHeight; ++z)
			if(collisionBlocks[heroColX + i][heroColY + z])	{
				float backwardsTime = 3.4;
				y += (GRAVITY_STRENGTH * pow(backwardsTime, 3) / 6) - (velY * backwardsTime);
				x += -velX * backwardsTime;
				velX = 0;
				velY = 0;
				jumping = 0;
				return;
			} // end if

} // end Hero::checkCollisions()

// draw Hero quad with Hero texture to back buffer
void Hero::draw(void) {

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	glTranslatef(x - camera->x, y - camera->y, 0.0);
	glBindTexture(GL_TEXTURE_2D, textures[HERO_TEXTURE + currentFrame]);

	glBegin(GL_QUADS);
	if(!movingLeft) {

	  	glTexCoord2f(0.0,   0.0);
  		glVertex2f(  0.0,   0.0);	 
  		glTexCoord2f(0.0,   1.0);
  		glVertex2f(  0.0,   height);	 
  		glTexCoord2f(1.0,   1.0);
  		glVertex2f(  width, height);	 
  		glTexCoord2f(1.0,   0.0);
  		glVertex2f(  width, 0.0);	 

	} else {

  		glTexCoord2f(1.0,   0.0);
  		glVertex2f(  0.0,   0.0);	 
  		glTexCoord2f(1.0,   1.0);
  		glVertex2f(  0.0,   height);	 
  		glTexCoord2f(0.0,   1.0);
  		glVertex2f(  width, height);	 
  		glTexCoord2f(0.0,   0.0);
  		glVertex2f(  width, 0.0);	 

	} // end if else
  	glEnd();
 
	glPopMatrix();	
	glDisable(GL_TEXTURE_2D);	

} // end Hero::draw()
