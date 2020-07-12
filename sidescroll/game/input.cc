#include "input.h"
#include "worldobject.h"
#include "mobileworldobject.h"
#include "hero.h"

extern float x, y;
extern Hero * hero;
extern void cleanUp(void);

void keyDown(unsigned char key, int px, int py) {
	switch (key) {
		case 27:
			cleanUp();
			exit(0);
			break;
		case 'a':
			hero->movingLeft = true;
			break;
		case 'd':
			hero->movingRight = true;
			break;
		case 'j':
			hero->jump(); 
			break;
	} // end switch
}

void keyUp(unsigned char key, int x, int y) {
	switch(key) {
		case 'a':
			hero->movingLeft = false;
			break;
		case 'd':
			hero->movingRight = false;
			break;
	} // end switch
}
