#include <vector>
#include <iostream>
#include <GL/glut.h>
#include "ltga.h"
#include "defines.h"
#include "input.h"
#include "textures.h"
#include "worldobject.h"
#include "fixedworldobject.h"
#include "ui.h"
#include "writetofile.h"
#include "viewport.h"
#include "textdisplayer.h"

int currentWindow, lastTime, numTextures;
int LEVEL_WIDTH = 800;
int LEVEL_HEIGHT = 600;
int COL_BLOCK_SIZE = 10;
char curLevFile[32];

UI ui;
Viewport viewport;

std::vector<FixedWorldObject *> vectorFWO[2];

GLuint 		 * textures	    = 0;
FixedWorldObject * fixedWorldObject = 0;
bool 		** collisionBlocks  = 0;

void idle(void) {
	int time     = glutGet(GLUT_ELAPSED_TIME);
	int delta    = time - lastTime;

	viewport.updateViewport(delta);
	glutPostWindowRedisplay(currentWindow);

	lastTime = time;
} // end idle()

void displayLevel(void) {
	ui.displayLevel();
} // end displayLevel()

void displayMenu(void) {
	ui.displayMenu();
} // end displayMenu()

void initWindow(void) {
	glutIgnoreKeyRepeat(0);
	
	glutDisplayFunc(displayLevel);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyDown);
	glutMouseFunc(mouseFunc);

  	glLoadIdentity();
	glOrtho(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT, 0, 1.0);
  	glMatrixMode(GL_MODELVIEW);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	currentWindow = glutGetWindow();

	glutSwapBuffers();
} // end initWindow()

void cleanUp(void) {
	for(int i = 0; i < 2; ++i) {
		if(!vectorFWO[0].empty()){
			for(int i = 0; i < vectorFWO[0].size(); ++i)
				delete vectorFWO[0][i];
			vectorFWO[0].clear();
		} // end if
	} 

	for(int i = 0; i < LEVEL_WIDTH / COL_BLOCK_SIZE; ++i)
		delete [] collisionBlocks[i];
	
	delete [] collisionBlocks;

	delete [] fixedWorldObject;

} // end cleanUp()

void fillCollisionBlocks(void) {

	collisionBlocks = new bool*[LEVEL_WIDTH / COL_BLOCK_SIZE];
	for(int i = 0; i < LEVEL_WIDTH / COL_BLOCK_SIZE; ++i) 
		collisionBlocks[i] = new bool[LEVEL_HEIGHT / COL_BLOCK_SIZE];

	for(int x = 0; x < LEVEL_WIDTH / COL_BLOCK_SIZE; ++x) 
		for(int y = 0; y < LEVEL_HEIGHT / COL_BLOCK_SIZE; ++y) 
			collisionBlocks[x][y] = false;
	
} // end fillCollisionBlocks()

bool getUserInput(void) {
	char tempChar;

	std::cout << "Welcome to the Level Editor!\n";
	std::cout << "Would you like to make a (n)ew level or (o)pen an existing level?\n";
	std::cin  >> tempChar;

	if(tempChar == 'n') {
	
		std::cout << "Please enter the new filename: (32 char max)\n";
		std::cin  >> curLevFile;
		std::cout << "What should " << curLevFile << "'s width be (in pixels)?\n";
		std::cin  >> LEVEL_WIDTH;
		std::cout << "What should " << curLevFile << "'s height be (in pixels)?\n";
		std::cin  >> LEVEL_HEIGHT;
		std::cout << "Finally, how big should " << curLevFile << "'s collision blocks be (in pixels)?\n";
		std::cin  >> COL_BLOCK_SIZE;

		fillCollisionBlocks();
		return true;

	} else if(tempChar == 'o') {

		std::cout << "Please enter the filename of level to open:\n";
		std::cin  >> curLevFile;
		readFromFile(curLevFile);
		return true;

	} else {

		std::cout << "\aFuck you asshole, the only valid options are (n)ew and (o)pen,\n";
		std::cout << "not " << tempChar << " like you pushed!  Try again!\n\n";
	 	return false;	

	} // end if else else

} // end getUserInput()

int main(int argc, char **argv) {

	while(!(getUserInput()));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutGameModeString("800x600:32@60");
	glutEnterGameMode();

	initWindow();
	loadTextures();
	initTextDisplayer();
	
	glutMainLoop();
	return 1;
}// end main()
