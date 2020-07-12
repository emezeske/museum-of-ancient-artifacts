#include <vector>
#include "GL/glut.h"
#include <iostream>
#include "input.h"
#include "worldobject.h"
#include "fixedworldobject.h"
#include "ui.h"
#include "writetofile.h"
#include "viewport.h"

extern char curLevFile[];

extern std::vector<FixedWorldObject *> vectorFWO[2];
extern FixedWorldObject * fixedWorldObject;

extern Viewport viewport;
extern UI ui;

extern void displayMenu();
extern void displayLevel();
extern void cleanUp();

void keyDown(unsigned char key, int px, int py) {

		switch (key) {
		case 27:
			cleanUp();
			exit(0);
			break;
		case '1':
			ui.showBackLayer = !ui.showBackLayer;
			break;
		case '2':
			ui.showFrontLayer = !ui.showFrontLayer;
			break;
		case '3':
			ui.showCollisions = !ui.showCollisions;
			break;
		case 'm':
			ui.switchDispMode();
			break;
		case 'w':
    			ui.moveUp(0);
			break;
		case 'W':
    			ui.moveUp(1);
			break;
		case 'a':
			ui.moveLeft(0);
			break;
		case 'A':
			ui.moveLeft(1);
			break;
		case 's':
    			ui.moveDown(0);
			break;
		case 'S':
    			ui.moveDown(1);
			break;
		case 'd':
			ui.moveRight(0);
			break;
		case 'D':
			ui.moveRight(1);
			break;
		case 'j':
			ui.selectPrevFWO();
			break;
		case 'k':
			ui.selectNextFWO();
			break;
		case 13:
			ui.newFWO();
			break;
		case 'y':
    			ui.deleteCurFWO();
			break;
		case 'r':
    			readFromFile(curLevFile);
			*(ui.currentFWO) = ui.selectedMenuItem = 0;
			break;
		case 't':
    			writeToFile(curLevFile);
 			break;
		case 'z':
			ui.showStatus = !ui.showStatus;
			break;
		case 'l':
			if(ui.backOrFrontFWO == 0) {
				ui.backOrFrontFWO = 1;
				ui.currentFWO = &ui.frontCurrentFWO;
			} else {
				ui.backOrFrontFWO  = 0;
				ui.currentFWO = &ui.backCurrentFWO;
			} // end if else
			break;
	} // end switch
} // end function keyDown()

void mouseFunc(int button, int state, int x, int y) {
	if(state == GLUT_DOWN)
		if(button == GLUT_LEFT_BUTTON)
			ui.mouseClick(x, y);
} // end mouseFunc()
