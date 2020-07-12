///////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
///////////////////////////////////////////////////////////////////////////////
#include <GL/glut.h>
#include "gameobject.h"

// had to make the GameObject and these funcs global so 
// we could register them with glut as callbacks :(
GameObject Game;

void idle(void) {
	Game.Frame();
} // end idle()

void display(void) {
	Game.ProcessGraphics();
} // end display()

void getKeyUp(unsigned char key, int x, int y) {
	Game.ProcessKeyUp(key, x, y);
} // end getKeyUp()

void getKeyDown(unsigned char key, int x, int y) {
	Game.ProcessKeyDown(key, x, y);
} // end getKeyDown()

void mouseMotionFunc(int x, int y) {
	Game.ProcessMouseMotion(x, y);
} // end mouseMotionFunc()

void mouseKeyFunc(int button, int state, int x, int y) {
	Game.ProcessMouseKey(button, state, x, y);
} // end mouseKeyFunc()

int main(int argc, char ** argv)
{		
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH);
	glutGameModeString("1024x768:32@60");
	glutEnterGameMode();

	Game.Initialize();

	return Game.Run();
}
