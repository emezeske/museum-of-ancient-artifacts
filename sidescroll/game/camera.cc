#include "defines.h"
#include "camera.h"
#include "worldobject.h"
#include "mobileworldobject.h"
#include "hero.h"

extern Hero * hero;

extern int LEVEL_WIDTH;
extern int LEVEL_HEIGHT;
extern int COL_BLOCK_SIZE;

// default constructor inits cam's x and y coords to center of screen
Camera::Camera(void) {
	x = hero->x - SCREEN_WIDTH / 2 + HERO_WIDTH / 2;
	y = hero->y - SCREEN_HEIGHT / 2 + HERO_HEIGHT / 2 ;
} // end Camera()

// move camera towards hero
void Camera::updateCamera(int delta) {

	x += (hero->x - x - SCREEN_WIDTH / 2 + HERO_WIDTH / 2) * CAMERA_CATCH_UP;
	if(x < 0)
		x = 0;
	if(x > LEVEL_WIDTH - SCREEN_WIDTH)
		x = LEVEL_WIDTH - SCREEN_WIDTH;

	y += (hero->y - y - SCREEN_HEIGHT / 2 + HERO_HEIGHT / 2) * CAMERA_CATCH_UP;
	if(y < 0)
		y = 0;
	if(y > LEVEL_WIDTH - SCREEN_WIDTH)
		y = LEVEL_WIDTH - SCREEN_WIDTH;

} // end updateCamera()
