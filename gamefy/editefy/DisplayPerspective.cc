#include "Display.h"
#include "defines.h"

EditefyDisplayPerspective::EditefyDisplayPerspective(EditefyWorld *w, double x, double y, double z, int posX, int posY, int width, int height):EditefyDisplay(w) {
	camera = new editefyPlayer(x, y, z);
	lx = ly = 0;
	sensitivity = 0.2;
	displayWidth = width;
	displayHeight = height;
	displayOffX = posX;
	displayOffY = posY;
} // end constructor EditefyDisplayPerspective()

EditefyDisplayPerspective::~EditefyDisplayPerspective() {
	delete camera;
} // end deconstructor EditefyDisplayPerspective()

void EditefyDisplayPerspective::PrepareToDraw() {
	SetViewport();

	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 4.0/3.0, 0.5, 100);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glRotatef( -camera->pitch, 1.0, 0.0, 0.0 );
	glRotatef( -camera->yaw,   0.0, 1.0, 0.0 );
	glTranslatef( -camera->x, -camera->y, -camera->z);
}

void EditefyDisplayPerspective::Draw() {
	PrepareToDraw();
	world->RenderScene();
} // end function Draw()

void EditefyDisplayPerspective::KeyDown(int key) {
	switch(key) {
		case 'w':
			camera->moveForward(true);
			break;
		case 's':
			camera->moveBackward(true);
			break;
		case 'a':
			camera->moveLeft(true);
			break;
		case 'd':
			camera->moveRight(true);
			break;
		case 'r':
			camera->moveUp(true);
			break;
		case 'f':
			camera->moveDown(true);
			break;
	} // end switch
} // end function KeyDown()

void EditefyDisplayPerspective::KeyUp(int key) {
	switch(key) {
		case 'w':
			camera->moveForward(false);
			break;
		case 's':
			camera->moveBackward(false);
			break;
		case 'a':
			camera->moveLeft(false);
			break;
		case 'd':
			camera->moveRight(false);
			break;
		case 'r':
			camera->moveUp(false);
			break;
		case 'f':
			camera->moveDown(false);
			break;
	} // end switch
} // end function KeyUp()

void EditefyDisplayPerspective::Mouse(int button, int isUp, int x, int y) {
	int modifiers = glutGetModifiers();
	switch(button) {
		case GLUT_LEFT_BUTTON:
		{
			if(!isUp)
			{
				unsigned short sel_obj_indx = SelectObject(x, y);
				if(sel_obj_indx != 0) // XXX FIXME
				{
					if(modifiers == GLUT_ACTIVE_SHIFT)
					{
						if(selection->IsInSelection(sel_obj_indx))
							selection->RemoveFromSelection(sel_obj_indx);
						else
							selection->AddToSelection(sel_obj_indx);
					} else
						selection->ClearSelection();
						selection->AddToSelection(sel_obj_indx);
				} else if(modifiers != GLUT_ACTIVE_SHIFT)
					selection->ClearSelection();
			}
		}
			break;
		case GLUT_RIGHT_BUTTON:
			if(!isUp) {
				ctrlState = CTRL_MOVING_CAMERA;
				lx = x;
				ly = y;
			} 
			else	ctrlState = CTRL_IDLE;
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
	} // end switch()
} // end function Mouse()

void EditefyDisplayPerspective::MouseMove(int x, int y) {
	switch(ctrlState) {
		case CTRL_MOVING_CAMERA:
			int cx = x - lx;
			int cy = y - ly;

			camera->yaw -= sensitivity*cx;
			camera->pitch -= sensitivity*cy;
			clampCamera();
			glutPostRedisplay();
			lx = x;
			ly = y;
			break;
	} // end switch()
} // end function MouseMove()

void EditefyDisplayPerspective::clampCamera() {
	if( camera->pitch > 90.0 )
		camera->pitch = 90.0;
	else if( camera->pitch < -90.0 )
		camera->pitch = -90.0;

	while( camera->yaw < 0.0 )
		camera->yaw += 360.0;
	while( camera->yaw >= 360.0 )
		camera->yaw -= 360.0;
} // end function clampCamera()

unsigned short EditefyDisplayPerspective::SelectObject(int x, int y) {
	/* XXX
	GLdouble m[16];
	GLdouble p[16];
	GLint	 v[4];
	GLdouble tX, tY, tZ;
	y = 2*halfHeight - y;

	// Apply the transformations to the matrices and store them
	// in arrays
	PrepareToDraw();
	glGetDoublev(GL_MODELVIEW_MATRIX, m);
	glGetDoublev(GL_PROJECTION_MATRIX, p);
	glGetIntegerv(GL_VIEWPORT, v);

	// Find the points in world space that correspond to the mouse's
	// x and y coords projected to the near and far clip planes
	gluUnProject((GLdouble)x, (GLdouble)y, 0.0, m, p, v, &tX, &tY, &tZ);
	Vector3 rayOrigin((GLdouble)tX, (GLdouble)tY, (GLdouble)tZ);
	gluUnProject((GLdouble)x, (GLdouble)y, 1.0, m, p, v, &tX, &tY, &tZ);
	Vector3 rayTerm((GLdouble)tX, (GLdouble)tY, (GLdouble)tZ);
	//Calculate the ray that is projected from the mouse click
	Vector3 rayVec = rayTerm - rayOrigin;

	wQuad *testQuad = world->sectors->quads;
	wQuad *selectedQuad = NULL;

	// used to determine which of multiple intersects is the correct one
	float minDistFromCamera = FLT_MAX;

	while(testQuad != NULL) {
		// Check to make sure the polygon is facing us and isn't
		// parallel to us
		if((rayVec / testQuad->normal) > 0) {
			// set up the plane equation for this quad ax + by + cz + d = 0
			float	a = testQuad->normal.x,
				b = testQuad->normal.y,
				c = testQuad->normal.z,
				d = -(	a*testQuad->vertex->pos.x +
					b*testQuad->vertex->pos.y +
					c*testQuad->vertex->pos.z),
				t = -(a*rayOrigin.x + b*rayOrigin.y + c*rayOrigin.z + d)/
				     (a*rayVec.x + b*rayVec.y + c*rayVec.z);
				if (t<0) goto throwoutquad;
			// calculate the point where this ray intersects with the plane
			Vector3 intersect(rayOrigin.x + t*rayVec.x,
					  rayOrigin.y + t*rayVec.y,
					  rayOrigin.z + t*rayVec.z);
			
			// temporary points (tp)
			Vector3 tp[4];
			// translate evertying so that the intersection point is at origin
			// and drop the major coordinate
			char dropAxis = 'x';
			Vector3 n( fabs(testQuad->normal.x), fabs(testQuad->normal.y), fabs(testQuad->normal.z) );

			if(n.x > n.y && n.x > n.z)
				dropAxis = 'x';
			else if(n.y > n.x && n.y > n.z)
				dropAxis = 'y';
			else if(n.z > n.x && n.z > n.y)
				dropAxis = 'z';

			for(int i = 0; i < 4; ++i) {
				switch(dropAxis) {
					case 'x':
						tp[i].x = testQuad->vertex[i].pos.z - intersect.z;
						tp[i].y = testQuad->vertex[i].pos.y - intersect.y;
						break;
					case 'y':
						tp[i].x = testQuad->vertex[i].pos.x - intersect.x;
						tp[i].y = testQuad->vertex[i].pos.z - intersect.z;
						break;
					case 'z':
						tp[i].x = testQuad->vertex[i].pos.x - intersect.x;
						tp[i].y = testQuad->vertex[i].pos.y - intersect.y;
						break;
				} // end switch()
			}
			// count how many times the polygon's outline intersects the positive x axis
			int num_intersects = 0;
			for(int cur = 0; cur < 4; ++cur) {
				int next = (cur+1)%4;
				if( (tp[cur].y > 0 && tp[next].y > 0) || (tp[cur].y < 0 && tp[next].y < 0) )
					continue;
				if(tp[cur].x > 0 && tp[next].x > 0)
				{
					++num_intersects;
					continue;
				} else if(tp[cur].x < 0 && tp[next].x < 0)
					continue;
				float slope = (tp[cur].y - tp[next].y)/(tp[cur].x-tp[next].x);
				if((tp[cur].x - tp[cur].y/slope) > 0.0)
					++num_intersects;
			}
			if(num_intersects & 0x0001)
			{
				float distFromCamera = sqrt(	pow(intersect.x - rayOrigin.x, 2) +
								pow(intersect.y - rayOrigin.y, 2) +
								pow(intersect.z - rayOrigin.z, 2) );
				if(distFromCamera < minDistFromCamera)
				{
					minDistFromCamera = distFromCamera;
					selectedQuad = testQuad;
				}
			}
		}
		throwoutquad:
		testQuad = testQuad->next;
	}
	return selectedQuad;
	*/
	return 0;
} // end function SelectQuad()

void EditefyDisplayPerspective::Reset() {
	camera->pitch = camera->yaw = 
	camera->x = camera->y = camera->z = 0.0 ;
	glutPostRedisplay();
} // end function Reset()

