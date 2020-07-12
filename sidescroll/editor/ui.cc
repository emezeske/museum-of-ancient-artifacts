#include <vector>
#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include "defines.h"
#include "ui.h"
#include "worldobject.h"
#include "fixedworldobject.h"
#include "viewport.h"
#include "textdisplayer.h"

extern int numTextures; 
extern int LEVEL_WIDTH;
extern int LEVEL_HEIGHT;
extern int COL_BLOCK_SIZE;

extern FixedWorldObject * fixedWorldObject;
extern std::vector<FixedWorldObject *> vectorFWO[2];
extern Viewport viewport;

extern void displayLevel();
extern void displayMenu();

extern bool ** collisionBlocks;

enum {FWO_BACK = 0, FWO_FRONT = 1};

UI::UI(void) {

	showMenu         = showStatus     = false;
	showBackLayer    = showFrontLayer = showCollisions = true;
	selectedMenuItem = backOrFrontFWO = 0;
	backCurrentFWO   = 0;
	frontCurrentFWO  = 0;
	currentFWO       = &backCurrentFWO;

} // end UI()

// iterate through the elements of the vector and call their display funcs
void UI::displayLevel(void) {

	glClear(GL_COLOR_BUFFER_BIT);

	if(showBackLayer)
		for(int i = 0; i < vectorFWO[FWO_BACK].size(); ++i) 
			vectorFWO[0][i]->draw(backCurrentFWO == i);

	if(showFrontLayer)
		for(int i = 0; i < vectorFWO[FWO_FRONT].size(); ++i) 
			vectorFWO[1][i]->draw(frontCurrentFWO == i);
	

	if(showCollisions)
		displayCollisionBlocks();

	drawAllText();
			
	glutSwapBuffers();

} // end displayLevel()

void UI::displayMenu(void) {

	glClear(GL_COLOR_BUFFER_BIT);
	
	int x = 0, y = 0;
	
	for(int i = 0; i < numTextures; ++i) {
		if(x == 8) {
			 x = 0;
			 ++y;
		}
		if(i == selectedMenuItem)
			fixedWorldObject[i].drawMenuTile(x, y, 1);	
		else
			fixedWorldObject[i].drawMenuTile(x, y, 0);	

		++x;
	} // end for

	drawAllText();

	glutSwapBuffers();

} // end displayMenu()

// this funciton draws little quads where we have decided collisions should occur
void UI::displayCollisionBlocks(void) {

	for(int x = 0; x < LEVEL_WIDTH / COL_BLOCK_SIZE; ++x)
		for(int y = 0; y < LEVEL_HEIGHT / COL_BLOCK_SIZE; ++y)
			if(collisionBlocks[x][y]) {
				glPushMatrix();
				glColor4f(0.0, 1.0, 0.0, 0.4);
				glTranslatef(x * COL_BLOCK_SIZE - viewport.x, y * COL_BLOCK_SIZE - viewport.y, 0.0);

				glBegin(GL_QUADS);
				glVertex2f(           0.0, 0.0);
				glVertex2f(           0.0, COL_BLOCK_SIZE);
				glVertex2f(COL_BLOCK_SIZE, COL_BLOCK_SIZE);
				glVertex2f(COL_BLOCK_SIZE, 0.0);
				glEnd();

				glPopMatrix();
			} // end if

} // end displayCollisionBlocks()

void UI::switchDispMode(void) {

	if(showMenu == false)
		glutDisplayFunc(::displayMenu);
	if(showMenu == true)
		glutDisplayFunc(::displayLevel);
	showMenu = !showMenu;

} // end switchDisplayMode()

void UI::mouseClick(int tempX, int tempY) {

	float x = (float)tempX;
	float y = (float)tempY;

	x = (float)(x / 1024 * SCREEN_WIDTH);
	y = (float)(SCREEN_HEIGHT - y / 768 * SCREEN_HEIGHT); 

	x = floorf( (viewport.x + x) / COL_BLOCK_SIZE );
	y = floorf( (viewport.y + y) / COL_BLOCK_SIZE );

	if(x >= 0)
		if(x < LEVEL_WIDTH / COL_BLOCK_SIZE)
			if(y >= 0)
				if(y < LEVEL_HEIGHT / COL_BLOCK_SIZE)
					collisionBlocks[(int)x][(int)y] = !collisionBlocks[(int)x][(int)y];

} // end mouseClick()

void UI::newFWO(void) {

	if(showMenu) {
		if(!vectorFWO[backOrFrontFWO].empty())
			vectorFWO[backOrFrontFWO].push_back(new FixedWorldObject(&fixedWorldObject[selectedMenuItem], vectorFWO[backOrFrontFWO][*currentFWO]) );
		else 
			vectorFWO[backOrFrontFWO].push_back(new FixedWorldObject(&fixedWorldObject[selectedMenuItem]) );
		*currentFWO = vectorFWO[backOrFrontFWO].size() - 1;
 		switchDispMode();
	} // end if

} // end newFWO()

void UI::deleteCurFWO(void) {

    	if(!vectorFWO[backOrFrontFWO].empty()) {
        	delete vectorFWO[backOrFrontFWO][*currentFWO];
    		vectorFWO[backOrFrontFWO].erase(vectorFWO[backOrFrontFWO].begin() + *currentFWO);
		if(*currentFWO)
			--(*currentFWO);
	}// end if

} // end deleteCurFWO()

void UI::selectNextFWO(void) {

	if(*currentFWO == vectorFWO[backOrFrontFWO].size() - 1)
		*currentFWO = 0;
	else if(!showMenu)
		++*currentFWO;

} // end selectNextFWO()

void UI::selectPrevFWO(void) {
	
	if(*currentFWO == 0)
		*currentFWO = vectorFWO[backOrFrontFWO].size() - 1;
	else if(!showMenu)
		--*currentFWO;

} // end selectPrevFWO()

void UI::moveLeft(bool shiftDown) {
	
	  if(showMenu){

		--selectedMenuItem;
		if(selectedMenuItem < 0)
			selectedMenuItem = numTextures - 1;

	  } else if(!vectorFWO[backOrFrontFWO].empty()) {
			if(shiftDown)
				vectorFWO[backOrFrontFWO][*currentFWO]->x -= 1;
	  		else
				vectorFWO[backOrFrontFWO][*currentFWO]->x -= 32;
	  	 } // end if
} // end moveLeft()

void UI::moveRight(bool shiftDown) {
	
	  if(showMenu){

		++selectedMenuItem;
		if(selectedMenuItem > numTextures - 1)
			selectedMenuItem = 0;

	  } else if(!vectorFWO[backOrFrontFWO].empty()) {
			if(shiftDown)
				vectorFWO[backOrFrontFWO][*currentFWO]->x += 1;
	  		else
				vectorFWO[backOrFrontFWO][*currentFWO]->x += 32;
	  	 } // end if

} // end moveRight()

void UI::moveUp(bool shiftDown) {
	
	  if(showMenu){

		if(selectedMenuItem + 8 < numTextures)
			selectedMenuItem += 8;
		else	
			selectedMenuItem = selectedMenuItem % 8;

	  } else if(!vectorFWO[backOrFrontFWO].empty()) {
			if(shiftDown)
				vectorFWO[backOrFrontFWO][*currentFWO]->y += 1;
	  		else
				vectorFWO[backOrFrontFWO][*currentFWO]->y += 32;
	  	 } // end if

} // end moveUp()

void UI::moveDown(bool shiftDown) {
	
	  if(showMenu){

		if(selectedMenuItem >= 8)
			selectedMenuItem -= 8;
		else {
			selectedMenuItem = numTextures - numTextures % 8 + selectedMenuItem % 8;
			if(selectedMenuItem >= numTextures)
				selectedMenuItem -= 8;
		}

	  } else if(!vectorFWO[backOrFrontFWO].empty()) {
			if(shiftDown)
				vectorFWO[backOrFrontFWO][*currentFWO]->y -= 1;
	  		else
				vectorFWO[backOrFrontFWO][*currentFWO]->y -= 32;
	  	 } // end if

} // end moveDown()
