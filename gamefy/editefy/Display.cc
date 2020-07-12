#include "Display.h"
#include "defines.h"

EditefyDisplay::EditefyDisplay(EditefyWorld *w):world(w) {
	ctrlState = CTRL_MOVING;
	mouseState = MS_NORMAL;
} // end constructor function EditefyDisplay()

void EditefyDisplay::SetViewport() {
	glViewport(displayOffX, displayOffY, displayWidth, displayHeight); 
} // end function SetViewport()

void EditefyDisplay::SetDisplayPosition(int x, int y) {
	displayOffX = x;
	displayOffY = y;
} // end function SetDisplayPosition()

void EditefyDisplay::SetDisplaySize(int w, int h) {
	displayWidth	= w;
	displayHeight	= h;
} // end function SetDisplaySize()

