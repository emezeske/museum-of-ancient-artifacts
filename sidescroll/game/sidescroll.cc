#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <GL/glut.h>
#include "ltga.h"
#include "defines.h"
#include "readlevelfile.h"
#include "input.h"
#include "textures.h"
#include "camera.h"
#include "worldobject.h"
#include "mobileworldobject.h"
#include "hero.h"
#include "fixedworldobject.h"

// the evil global variables:
int lastTime   , numTextures , currentWindow;
int LEVEL_WIDTH, LEVEL_HEIGHT, COL_BLOCK_SIZE;

Hero 		 * hero;
GLuint 		 * textures;
FixedWorldObject * fixedWorldObject;
Camera 		 * camera;
bool            ** collisionBlocks;

std::vector<WorldObject *> pWorldObjectVector[2];

void idle(void) {
	int time = glutGet(GLUT_ELAPSED_TIME);
	int delta = time - lastTime;
	
	hero->updateState(delta);
	camera->updateCamera(delta);

	lastTime = time;
	glutPostWindowRedisplay(currentWindow);
} // end idle()

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	
	for(int i = 0; i < pWorldObjectVector[0].size(); ++i) 
		pWorldObjectVector[0][i]->draw();

	for(int i = 0; i < pWorldObjectVector[1].size(); ++i) 
		pWorldObjectVector[1][i]->draw();

	glutSwapBuffers();
} // end display()

void initWindow(void) {
	glutIgnoreKeyRepeat(1);
	
	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutIdleFunc(idle);

  	glMatrixMode(GL_PROJECTION);
  	glLoadIdentity();
	glOrtho(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT, 0, 1.0);
  	glMatrixMode(GL_MODELVIEW);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glutSetCursor(GLUT_CURSOR_NONE);

	currentWindow = glutGetWindow();

	glutSwapBuffers();
} // end initWindow()

void initGame(void) {

	// initialize game objects here
	readFromFile();
	hero = new Hero(10.0, 600.0, 0.0, 0.0);
	pWorldObjectVector[0].push_back(hero);
	camera = new Camera();

} // end initGame

void cleanUp(void) {

	for(int which = 0; which < 2; ++which) {
		if(!pWorldObjectVector[which].empty()){
			for(int i = 0; i < pWorldObjectVector[which].size(); ++i)
				delete pWorldObjectVector[which][i];
			pWorldObjectVector[which].clear();
		} // end if
	} 

	delete camera;

	for(int i = 0; i < LEVEL_WIDTH / COL_BLOCK_SIZE; ++i)
		delete collisionBlocks[i];
	delete collisionBlocks;

} // end cleanUp()

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	//glutGameModeString("800x600:32@60");
	//glutEnterGameMode();
	glutCreateWindow( "wacky" );
	glutReshapeWindow( 800, 600 );

	initWindow();
	loadTextures();
	initGame();
	
	glutMainLoop();
	return 0;
}// end main()
