#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include "../shared/gCWorld.h"
#include "../shared/ltga.h"
#include "Player.h"
#include "Frustum.h"

float sensitivity =	0.2;			// mouse sensitivity
float frameRate = 0.0;
int viewPortCenterX =	-1;
int viewPortCenterY =	-1;
int lastTime =		0;
int lastFrameTime =	0;
Player 	*player;
gCWorld *world;
Frustum *viewFrustum;

void quit() {
	delete player;
	delete world;
	delete viewFrustum;
	exit(0);
} // end function quit()

int setupOpenGL() {
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
//	glEnable(GL_FOG);
//	GLfloat density;
//	GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};
//	glFogi(GL_FOG_MODE, GL_EXP);
//	glFogfv(GL_FOG_COLOR, fogColor);
//	glFogf(GL_FOG_DENSITY, 0.025);
//	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glClearColor(1.0, 0.5, 0.5, 1.0);
} // end function setupOpenGL()

void moveThings(int ms) {
	// move player
	player->move(ms);
	// return
} // end function moveThings()

void extractFrustumPlanes(Frustum *f, const Matrix4x4 &m) {
	// left frustum plane
	f->left.n.x	= m._41 + m._11;
	f->left.n.y	= m._42 + m._12;
	f->left.n.z	= m._43 + m._13;
	f->left.d	= m._44 + m._14;
	f->left.Normalize();
	// right frustum plane
	f->right.n.x	= m._41 - m._11;
	f->right.n.y	= m._42 - m._12;
	f->right.n.z	= m._43 - m._13;
	f->right.d	= m._44 - m._14;
	f->right.Normalize();
	// bottom frustum plane
	f->bottom.n.x	= m._41 + m._21;
	f->bottom.n.y	= m._42 + m._22;
	f->bottom.n.z	= m._43 + m._23;
	f->bottom.d	= m._44 + m._24;
	f->bottom.Normalize();
	// top frustum plane
	f->top.n.x	= m._41 - m._21;
	f->top.n.y	= m._42 - m._22;
	f->top.n.z	= m._43 - m._23;
	f->top.d	= m._44 - m._24;
	f->top.Normalize();
	// near frustum plane
	f->near.n.x	= m._41 + m._31;
	f->near.n.y	= m._42 + m._32;
	f->near.n.z	= m._43 + m._33;
	f->near.d	= m._44 + m._34;
	f->near.Normalize();
	// far frustum plane
	f->far.n.x	= m._41 - m._31;
	f->far.n.y	= m._42 - m._32;
	f->far.n.z	= m._43 - m._33;
	f->far.d	= m._44 - m._34;
	f->far.Normalize();
	// precalculate bounding box effective radii
	f->calculateEffectiveRadii(world->treeRoot->boundingBox);
}// end function extractFrustumPlanes()

void gDisplay() {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW_MATRIX);
	glLoadIdentity();

	glRotatef( -player->pitch, 1.0, 0.0, 0.0 );
	glRotatef( -player->yaw,   0.0, 1.0, 0.0 );
	//world->renderSkybox();
	glTranslatef( -player->x, -player->y, -player->z);

	Matrix4x4 *combo;			// the projection times the modelview matrices

	float   proj[16];								// This will hold our projection matrix
	float   modl[16];								// This will hold our modelview matrix
	float   clip[16];								// This will hold the clipping planes

	glGetFloatv( GL_PROJECTION_MATRIX, proj );
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	combo = (Matrix4x4*)clip;
//	static bool truckanjka = true;
//	if(truckanjka) {
		extractFrustumPlanes(viewFrustum, *combo);
//		truckanjka = false;
//	} // end if

	glEnable(GL_TEXTURE_2D);
	world->renderScene();

	glutSwapBuffers();
} // end function gDisplay()

void gIdle() {
	// ms is the time it took to render last frame
	int ms = glutGet(GLUT_ELAPSED_TIME)-lastTime;
	moveThings(ms);
	glutWarpPointer(viewPortCenterX, viewPortCenterY);
	glutPostRedisplay();
	lastTime = glutGet(GLUT_ELAPSED_TIME);
} // end function gIdle()

void clampCamera() {
	if( player->pitch > 90.0 )
		player->pitch = 90.0;
	else if( player->pitch < -90.0 )
		player->pitch = -90.0;

	while( player->yaw < 0.0 )
		player->yaw += 360.0;
	while( player->yaw >= 360.0 )
		player->yaw -= 360.0;
} // end function clampCamera()

void gKeyDown(unsigned char key, int, int) {
	switch(key) {
		case 'q':
		case 'Q':
		case 27:
			quit();
			break;
		case 'w':
			player->moveForward(true);
			break;
		case 's':
			player->moveBackward(true);
			break;
		case 'a':
			player->moveLeft(true);
			break;
		case 'd':
			player->moveRight(true);
			break;
		case ' ':
			player->jump(true);
			break;

	} // end switch
} // end function gKeyDown()

void gKeyUp(unsigned char key, int, int) {
	switch(key) {
		case 'w':
			player->moveForward(false);
			break;
		case 's':
			player->moveBackward(false);
			break;
		case 'a':
			player->moveLeft(false);
			break;
		case 'd':
			player->moveRight(false);
			break;
		case ' ':
			player->jump(false);
			break;
	} // end switch
} // end function gKeyUp()

void initGame() {
	player 		= new Player(0.0, 0.5, 0.0);
	world 		= new gCWorld("./cworld");
	viewFrustum 	= new Frustum();
} // end function initGame()

void initMouse() {

	if(viewPortCenterX<0) {
		viewPortCenterX = glutGet(GLUT_WINDOW_WIDTH)/2;
		viewPortCenterY = glutGet(GLUT_WINDOW_HEIGHT)/2;
	} // end if

	glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(viewPortCenterX, viewPortCenterY);

} // end function initMouse()

void gMouseMove(int x, int y) {
	int cx=0, cy =0;
	cx = x-viewPortCenterX;
	cy = y-viewPortCenterY;
	player->yaw -= sensitivity*cx;
	player->pitch -= sensitivity*cy;
	clampCamera();
	glutPostRedisplay();
} // end function gMouseMove()

void gReshape(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLdouble)width/(GLdouble)height, 0.5, 100);
	glMatrixMode(GL_MODELVIEW);
	viewPortCenterX = width/2;
	viewPortCenterY = height/2;
} // end function gReshape()

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	//glutGameModeString("1024x768:32@85");
	//glutEnterGameMode();
	glutCreateWindow( "wacky" );
	glutReshapeWindow( 1024, 768 );
	setupOpenGL();
	initGame();
	lastFrameTime = lastTime = glutGet(GLUT_ELAPSED_TIME);
	initMouse();
	glutDisplayFunc(gDisplay);
	glutIdleFunc(gIdle);
	glutKeyboardFunc(gKeyDown);
	glutKeyboardUpFunc(gKeyUp);
	glutPassiveMotionFunc(gMouseMove);
	glutMouseFunc(NULL);
	glutMotionFunc(gMouseMove);
	glutReshapeFunc(gReshape);
	glutMainLoop();
	return 0;
} // end function main()

