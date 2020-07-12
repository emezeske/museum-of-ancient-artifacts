#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include "textures.h"
#include "skier.h"
#include "tree.h"

using namespace std;

int currentWindow;
GLuint * textures[MAX_TEXTURES];
Tree   * trees[MAX_TREES];
Skier  * skier;

void idle(void) {
	static int lastTime;
	static int time = glutGet(GLUT_ELAPSED_TIME);
	static int delta = time - lastTime;
	lastTime = time;

	skier->update(delta);
	glutPostWindowRedisplay(currentWindow);
}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT);

	skier->draw();

	for(int i = 0; i < MAX_TREES; ++i)
		trees[i]->draw();

	glutSwapBuffers();

} // end display()

void keyboardDown(unsigned char key, int x, int y) {

	switch(key) {
		case 27:
			delete skier;
			exit(0);
			break;
		case 'a':
			skier->movingLeft = true;
			break;
		case 'd':
			skier->movingRight = true;
			break;
		case 'w':
			skier->slowingDown = true;
			break;
	} // end switch(key)

} // end keyboardDown()

void keyboardUp(unsigned char key, int x, int y) {

	switch(key) {
		case 'a':
			skier->movingLeft = false;
			break;
		case 'd':
			skier->movingRight = false;
			break;
		case 'w':
			skier->slowingDown = false;
			break;
	} // end switch(key)

} // end keyboardUp()

void initGame(void) {

	skier = new Skier();

	srand((unsigned)time(NULL));
	for(int i = 0; i < MAX_TREES; ++i) 
		trees[i] = new Tree();

} // end initGame()

int main(int argc, char ** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutGameModeString("800x600:32@60");
	glutEnterGameMode();

	currentWindow = glutGetWindow();
	glLoadIdentity();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	glOrtho(0, 800, 0, 600, 0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	loadTextures();
	initGame();

	glutMainLoop();

	return 0;
} // end main()
