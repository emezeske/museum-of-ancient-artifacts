#include <vector>
#include <iostream>
#include "defines.h"
#include "viewport.h"
#include "worldobject.h"
#include "fixedworldobject.h"
#include "ui.h"

extern int LEVEL_WIDTH;
extern int LEVEL_HEIGHT;
extern int COL_BLOCK_SIZE;

extern std::vector<FixedWorldObject *> vectorFWO[2];
extern UI ui;

// default constructor
Viewport::Viewport(void) {

	x = 0;
	y = 0;

} // end Viewport()

void Viewport::updateViewport(int delta) {
	
	if(!vectorFWO[ui.backOrFrontFWO].empty())
		x = vectorFWO[ui.backOrFrontFWO][*(ui.currentFWO)]->x - (SCREEN_WIDTH / 2)
			+ vectorFWO[ui.backOrFrontFWO][*(ui.currentFWO)]->width / 2;
	if(x < 0)
		x = 0;
	if(x >= LEVEL_WIDTH - SCREEN_WIDTH)
		x = LEVEL_WIDTH - SCREEN_WIDTH - 1;

	if(!vectorFWO[ui.backOrFrontFWO].empty())
		y = vectorFWO[ui.backOrFrontFWO][*(ui.currentFWO)]->y - (SCREEN_WIDTH / 2)
			+ vectorFWO[ui.backOrFrontFWO][*(ui.currentFWO)]->width / 2;
	if(y < 0)
		y = 0;
	if(y >= LEVEL_HEIGHT - SCREEN_HEIGHT)
		y = LEVEL_HEIGHT - SCREEN_HEIGHT - 1;

} // end updateViewport()
