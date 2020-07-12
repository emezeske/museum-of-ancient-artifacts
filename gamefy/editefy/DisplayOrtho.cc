#include "Display.h"
#include "defines.h"

bool	EditefyDisplayOrtho::SnapToGrid =	true;
bool	EditefyDisplay::leftButton =		0;
bool	EditefyDisplay::middleButton =		0;
bool	EditefyDisplay::rightButton =		0;
double	EditefyDisplayOrtho::editPlaneX =	0.0;
double	EditefyDisplayOrtho::editPlaneY =	0.0;
double	EditefyDisplayOrtho::editPlaneZ =	0.0;

EditefyDisplayOrtho::EditefyDisplayOrtho(EditefyWorld *w, int view, int width, int height):EditefyDisplay(w) {
	OrthoView = view;
	zoom = 1.0;
	GridSize = 1.0;
	offX = offY = 0.0;
	updateExtents();
	adjustingEditingPlane = 0;
	editPlaneDeltaAccum = 0;
	displayWidth = width;
	displayHeight = height;
} // end constructor EditefyDisplayOrtho()

// called whenever the display's extent has been changed
void EditefyDisplayOrtho::updateExtents() {
	left	= -40*zoom+offX;
	right	=  40*zoom+offX;
	bottom	= -30*zoom+offY;
	top	=  30*zoom+offY;
} // end function updateExtents()

double EditefyDisplayOrtho::GridSnap(double n) {
	double hgs = GridSize*0.5;
	return floor((n+hgs)/GridSize)*GridSize;
} // end function GridSnap()

Vector3 EditefyDisplayOrtho::GridSnap(Vector3 p) {
	Vector3 snapP(GridSnap(p.x), GridSnap(p.y), GridSnap(p.z));
	return snapP;
} // end function GridSnap()

void EditefyDisplayOrtho::DrawGrid() {
	double	vW = 80*zoom,
		vH = 60*zoom;
	int	theLeftLine = (int)(left/GridSize),
		theBottomLine = (int)(bottom/GridSize);
 
	glColor3f(0.3, 0.3, 0.3);
	float skip = 1.0;
	while(GridSize/(top-bottom)*skip<0.005) skip*=8.0;


	glBegin(GL_LINES);
		for(double r = theBottomLine*GridSize; r < top; r+=GridSize*skip) {
			glVertex2f(left, r);
			glVertex2f(right, r);
		} // end for r
		for(double c = theLeftLine*GridSize; c < right; c+=GridSize*skip) {
			glVertex2f(c, bottom);
			glVertex2f(c, top);
		} // end for c
	glEnd();

} // end function DrawGrid()

void EditefyDisplayOrtho::DrawEditPlanes() {
	glColor3f(0.0, 0.6, 0.6);
	glBegin(GL_LINES);
		switch(OrthoView) {
			case ORTHO_TOP:
				glVertex2f(left, editPlaneZ);
				glVertex2f(right, editPlaneZ);
				glVertex2f(editPlaneX, bottom);
				glVertex2f(editPlaneX, top);
				break;
			case ORTHO_SIDE:
				glVertex2f(left, editPlaneY);
				glVertex2f(right, editPlaneY);
				glVertex2f(editPlaneZ, bottom);
				glVertex2f(editPlaneZ, top);
				break;
			case ORTHO_FRONT:
				glVertex2f(left, editPlaneY);
				glVertex2f(right, editPlaneY);
				glVertex2f(editPlaneX, bottom);
				glVertex2f(editPlaneX, top);
				break;
		} // end switch()
	glEnd();
} // end function DrawEditPlanes()

void EditefyDisplayOrtho::Draw() {
	SetViewport();
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, -100, 100);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	DrawGrid();
	DrawEditPlanes();

	switch(OrthoView) {
		case ORTHO_TOP:
			glRotatef( -90, 1.0, 0.0, 0.0 );
			break;
		case ORTHO_SIDE:
			glRotatef( 90, 0.0, 1.0, 0.0 );
			break;
		case ORTHO_FRONT:
			break;
	} // end switch()
	world->RenderSketch();
} // end function Draw()

void EditefyDisplayOrtho::KeyDown(int key) {
	int modifiers = glutGetModifiers();
	if(modifiers==0)
		switch(key) {
			case '[':
				GridSize*=2.0;
				break;
			case ']':
				if(GridSize>=0.125) GridSize*=0.5;
				break;
			case '\\':
				SnapToGrid = !SnapToGrid;
				break;
			case 'm':
				ctrlState = CTRL_MOVING;
				break;
			case 's':
				ctrlState = CTRL_SCALING;
				break;
			case 'r':
				ctrlState = CTRL_ROTATING;
				break;
		} // end switch()
} // end function KeyDown()

void EditefyDisplayOrtho::KeyUp(int) {
} // end function KeyUp()

Vector3 EditefyDisplayOrtho::MouseToWorld(int x, int y) {
	y = displayHeight - y;		// mouse origin is upper left
	x -= displayOffX+displayWidth/2;
	y -= displayOffY-displayHeight/2;
	Vector3 p;
	switch(OrthoView) {
		case ORTHO_TOP:
			p.x = (x/(double)displayWidth) * (right-left) + offX;
			p.y = editPlaneY; 
			p.z = ((y/(double)displayHeight) * (top-bottom) + offY);
			break;
		case ORTHO_SIDE:
			p.x = editPlaneX;
			p.y = (y/(double)displayHeight) * (top-bottom) + offY; 
			p.z = (x/(double)displayWidth) * (right-left) + offX;
			break;
		case ORTHO_FRONT:
			p.x = (x/(double)displayWidth) * (right-left) + offX;
			p.y = (y/(double)displayHeight) * (top-bottom) + offY; 
			p.z = editPlaneZ;
			break;
	} // end switch()
	if(SnapToGrid) return GridSnap(p);
	else return p;
} // end function MouseToWorld()

void EditefyDisplayOrtho::Mouse(int button, int isUp, int x, int y) {
	lx = x;
	ly = y;
	if(isUp&&button!=3&&button!=4) {
		switch(button) {
			case GLUT_LEFT_BUTTON:
				leftButton = false;
				break;
			case GLUT_RIGHT_BUTTON:
				adjustingEditingPlane = 0;
				mouseState = MS_NORMAL;
				rightButton = false;
				MouseStartMoving(x,y);
				break;
			case GLUT_MIDDLE_BUTTON:
				ctrlState = lastCtrlState;
				middleButton = false;
		} // end switch()
	} else {
		switch(button) {
			case GLUT_LEFT_BUTTON:
				leftButton = true;
				switch(ctrlState) {
					case CTRL_SCALING:
						break;
					case CTRL_MOVING:
						MouseStartMoving(x, y);
						break;
				} // end switch()
				break;
			case GLUT_RIGHT_BUTTON:
				rightButton = true;
				adjustingEditingPlane = 1;
				mouseState = MS_EPLANE;
				break;
			case GLUT_MIDDLE_BUTTON:
				lastCtrlState = ctrlState;
				ctrlState = CTRL_PANNING;
				middleButton = true;
				break;
			case 3:			// zoom in
				zoom *= 0.8;
				updateExtents();
				glutPostRedisplay();
				break;
			case 4:			// zoom out
				zoom *= 1.25;
				updateExtents();
				glutPostRedisplay();
				break;
		} // end switch()
	} // end if
} // end function Mouse()

// called when the user presses the left mouse button while ctrlState == CTRL_MOVING
void EditefyDisplayOrtho::MouseStartMoving(int x, int y) {
	tmpPts[0] = MouseToWorld(x, y);
} // end function MouseStartMoving()

void EditefyDisplayOrtho::MouseMoving(int x, int y) {
	tmpPts[1] = MouseToWorld(x,y);
	tmpPts[2] = tmpPts[1]-tmpPts[0];
	//selection->MoveSelection(tmpPts[2]);
	tmpPts[0] = tmpPts[1];
} // end function MouseMoving()

void EditefyDisplayOrtho::MouseScaling(int x) {
	float percentPerPixel = 0.05;
	switch(OrthoView) {
		case ORTHO_SIDE:
			//selection->Scale(1 + x*percentPerPixel, 1 + x*percentPerPixel, 1 + x*percentPerPixel);
			break;
		case ORTHO_TOP:
			//selection->Scale(1 + x*percentPerPixel, 1 + x*percentPerPixel, 1 + x*percentPerPixel);
			break;
		case ORTHO_FRONT:
			//selection->Scale(1 + x*percentPerPixel, 1 + x*percentPerPixel, 1 + x*percentPerPixel);
			break;
	} // end switch()
} // end function MouseScaling()

void EditefyDisplayOrtho::MouseRotating(int x) {
	// Number of degrees to rotate object per pixel of mouse motion
	int degreesPerPixel = 15;
	switch(OrthoView) {
		case ORTHO_SIDE:
			selection->RotateXYZ((float)x * degreesPerPixel * M_PI/180, 0.0, 0.0);
			break;
		case ORTHO_TOP:
			selection->RotateXYZ(0.0, (float)x * degreesPerPixel * M_PI/180, 0.0);
			break;
		case ORTHO_FRONT:
			selection->RotateXYZ(0.0, 0.0, (float)x * degreesPerPixel * M_PI/180);
			break;
	} // end switch()
} // end function MouseRotating()

void EditefyDisplayOrtho::MousePanning(int x, int y) {
	offX-= x*((80*zoom)/displayWidth);
	offY+= y*((60*zoom)/displayHeight);
	updateExtents();
	glutPostRedisplay();
} // end function MousePanning()

void EditefyDisplayOrtho::MouseMove(int x, int y) {
	int cx = x - lx;
	int cy = y - ly;
	if(adjustingEditingPlane)
		if(SnapToGrid) {
			editPlaneDeltaAccum += cy;
			if(abs(editPlaneDeltaAccum) > EDIT_PLANE_ADJUST_SENSITIVITY) {
				int Flimbobinator = editPlaneDeltaAccum / EDIT_PLANE_ADJUST_SENSITIVITY;
				switch(OrthoView) {
					case ORTHO_SIDE:
						editPlaneX = GridSnap(editPlaneX);
						editPlaneX += Flimbobinator*GridSize;
						break;
					case ORTHO_TOP:
						editPlaneY = GridSnap(editPlaneY);
						editPlaneY += Flimbobinator*GridSize;
						break;
					case ORTHO_FRONT:
						editPlaneZ = GridSnap(editPlaneZ);
						editPlaneZ += Flimbobinator*GridSize;
						break;
				} // end switch
				editPlaneDeltaAccum = editPlaneDeltaAccum%10;
				glutPostRedisplay();
			} // end if
		} else {
			switch(OrthoView) {
				case ORTHO_SIDE:
					editPlaneX += cy*((60*zoom)/displayHeight);
					break;
				case ORTHO_TOP:
					editPlaneY += cy*((60*zoom)/displayHeight);
					break;
				case ORTHO_FRONT:
					editPlaneZ += cy*((60*zoom)/displayHeight);
					break;
			} // end switch
			glutPostRedisplay();
		} // end if
	else switch(ctrlState) {
		case CTRL_PANNING:
			MousePanning(cx, cy);
			break;
		case CTRL_MOVING:
			if(mouseState == MS_NORMAL)
				MouseMoving(x, y);
			break;
		case CTRL_SCALING:
			if(mouseState == MS_NORMAL)
				MouseScaling(cx);
			break;
		case CTRL_ROTATING:
			if(mouseState == MS_NORMAL)
				MouseRotating(cx);
			break;
	} // end switch(ctrlState)
	lx = x;
	ly = y;
} // end function MouseMove()

void EditefyDisplayOrtho::Reset() {
	offX = offY = 0.0;
	zoom = 1.0;
	GridSize = 1.0;
	updateExtents();
	glutPostRedisplay();
} // end function Reset()
