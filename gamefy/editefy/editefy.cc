#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include "World.h"
#include "../shared/ltga.h"
#include "Display.h"
#include "defines.h"
#include "selection.h"
#include "Object3D.h"

EditefyWorld 			*world = NULL;
EditefyDisplayPerspective	*dispPerspective = NULL;
EditefyDisplayOrtho		*dispOrthoTOP = NULL;
EditefyDisplayOrtho		*dispOrthoFRONT = NULL;
EditefyDisplayOrtho		*dispOrthoSIDE = NULL;
EditefyDisplay			*dispCurrent = NULL;
EditefySelection 		*selection = NULL;

bool mouseButtonDown = false;
char filename[256];
int lx=0, ly =0;
int mouseX=0, mouseY=0;
int lastTime = 0;
int EditingMode = MODE_VERTEX;
//int ControlMode = CTRL_ADDING;

int halfHeight	= 512;
int halfWidth	= 384;
// Change this variable to true to run in fullscreen mode
bool fullscreen = false;

void printDebugInfo() {
	printf("control state = %d\n", dispCurrent->ctrlState);
} // end function printDebugInfo()

void quit() {
	delete	dispPerspective,
		dispOrthoTOP,
		dispOrthoSIDE,
		dispOrthoFRONT,
		selection,
		world;
	exit(1);
} // end function quit()

int setupOpenGL() {
	glClearDepth(10.0);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0, 0.0, 0.0, 1.0);
} // end function setupOpenGL()

void toggleFullscreen() {
	fullscreen = !fullscreen;
	if(fullscreen) {
		glutGameModeString("1024x768:32@60");
		glutEnterGameMode();
	} else {
		glutCreateWindow("Editefy");
		glutReshapeWindow(800, 600);
	} // end if
} // end function toggleFullscreen()

void gDisplay() {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// PERSPECTIVE, LOWER LEFT
	dispPerspective->Draw();

	// ORTHOGONAL TOP, UPPER RIGHT
	dispOrthoTOP->Draw();
		
	// ORTHOGONAL FRONT, UPPER LEFT 
	dispOrthoFRONT->Draw();
		
	// ORTHOGONAL SIDE, LOWER RIGHT
	dispOrthoSIDE->Draw();
	
	glutSwapBuffers();
} // end function gDisplay()

EditefyDisplay *whichDisplay(int x, int y) {
	if(x>halfWidth) {
		if(y<halfHeight) { // upper right
			return (EditefyDisplay*)dispOrthoTOP;
		} else {		// lower right
			return (EditefyDisplay*)dispOrthoSIDE;
		}
	} else {
		if(y<halfHeight) { // upper left
			return (EditefyDisplay*)dispOrthoFRONT;
		} else {		// lower left
			return (EditefyDisplay*)dispPerspective;
		}
	}
} // end function whichDisplay()

void gKeyDown(unsigned char key, int x, int y) {
	int modifiers = glutGetModifiers();
	if(modifiers!=GLUT_ACTIVE_CTRL)
		switch(key) {
			case 27:
				quit();
				return;
			case '`':
				dispCurrent->Reset();
				return;
			case '~':
				dispOrthoTOP->Reset();
				dispOrthoFRONT->Reset();
				dispOrthoSIDE->Reset();
				dispPerspective->Reset();
				return;
			case GLUT_KEY_DELETE:
				selection->DeleteSelection();
				return;
			case '!':
				printDebugInfo();
				return;
		} // end switch(key)
	else if(modifiers==GLUT_ACTIVE_CTRL)
		switch(key+96) { // goddammit 96 seems to be CTRL
			case 's':
				// XXX WRITE WORLD TO FILE
				//world->writeToFile("./outworld");
				return;
			case 'd':
				selection->DuplicateSelection();
				return;
		} // end switch(key)
	// if nothing claimed the keypress, send it to the current active display
		dispCurrent->KeyDown(key);
} // end function gKeyDown()

void gKeyUp(unsigned char key, int, int) {
	dispCurrent->KeyUp(key);
} // end function gKeyUp()

// stoopid 
void gSpecialKeyUp(int key, int x, int y) {
	dispCurrent->KeyUp(key);
}

// ghey!
void gSpecialKeyDown(int key, int x, int y) {
	switch(key) {
	} // end switch
	dispCurrent->KeyUp(key);
}

void gMouse(int button, int isUp, int x, int y) {
	dispCurrent->Mouse(button, isUp, x, y);
	dispCurrent = whichDisplay(x, y);
	mouseButtonDown = 1-isUp;
} // end function gMouse()

void gMouseMove(int x, int y) {
	dispCurrent->MouseMove(x, y);
} // end function gMouseMove()

void gPassiveMouseMove(int x, int y) {
	if(!mouseButtonDown)
		dispCurrent = whichDisplay(x, y);
} // end function gPassiveMouseMove()

void gReshape(int width, int height) {
	halfWidth  = width/2;
	halfHeight = height/2;
	dispOrthoTOP->SetDisplayPosition(halfWidth, halfHeight);
	dispOrthoSIDE->SetDisplayPosition(halfWidth, 0);
	dispOrthoFRONT->SetDisplayPosition(0, halfHeight);
	dispOrthoTOP->SetDisplaySize(halfWidth, halfHeight);
	dispOrthoSIDE->SetDisplaySize(halfWidth, halfHeight);
	dispOrthoFRONT->SetDisplaySize(halfWidth, halfHeight);
	dispPerspective->SetDisplaySize(halfWidth, halfHeight);
	dispCurrent = dispPerspective;
} // end function gReshape()

void gIdle() {
	// ms is the time it took to render last frame
	int ms = glutGet(GLUT_ELAPSED_TIME)-lastTime;
	dispPerspective->camera->move(ms);
	glutPostRedisplay();
	lastTime = glutGet(GLUT_ELAPSED_TIME);
} // end function gIdle()

void initGame() {
	world = 			new EditefyWorld();
	selection =			new EditefySelection();
	dispPerspective =		new EditefyDisplayPerspective(world, 0.0, 0.0, 0.0, 0, 0, halfWidth, halfHeight);
	dispOrthoTOP =			new EditefyDisplayOrtho(world, ORTHO_TOP, halfWidth, halfHeight);
	dispOrthoSIDE =			new EditefyDisplayOrtho(world, ORTHO_SIDE, halfWidth, halfHeight);
	dispOrthoFRONT =		new EditefyDisplayOrtho(world, ORTHO_FRONT, halfWidth, halfHeight);
	world->LoadLevel((const char*)filename);
	world->LoadMaterials();
	lastTime = glutGet(GLUT_ELAPSED_TIME);
} // end function initGame()

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	if(argc == 1)
		strcpy(filename, "./world");
	else
		strcpy(filename, argv[1]);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	if(fullscreen) {
		glutGameModeString("1024x768:32@60");
		glutEnterGameMode();
	} else {
		glutCreateWindow("Editefy");
		glutReshapeWindow(800, 600);
	}
	setupOpenGL();
	initGame();
	glutDisplayFunc(gDisplay);
	glutKeyboardFunc(gKeyDown);
	glutKeyboardUpFunc(gKeyUp);
	glutSpecialFunc(gSpecialKeyDown);
	glutSpecialUpFunc(gSpecialKeyUp);
	glutMouseFunc(gMouse);
	glutMotionFunc(gMouseMove);
	glutPassiveMotionFunc(gPassiveMouseMove);
	glutReshapeFunc(gReshape);
	glutIdleFunc(gIdle);
	glutMainLoop();
	return 0;
} // end function main()


