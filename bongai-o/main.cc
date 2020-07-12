#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include "defines.h"
#include "ltga.h"
#include "spacecraft.h"
#include "network.h"

using namespace std;

int lastTime;
GLuint * textures;
SpaceCraft * craft, *evilcraft;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	// Determine the texture coordinates of the background
	float x = craft->x;
	float y = craft->y;

	float 	fracPartX = x - floorf(x),
		fracPartY = y - floorf(y);

	float 	texX = ((int)(x)%SCREEN_WIDTH + fracPartX)/SCREEN_WIDTH,
		texY = ((int)(y)%SCREEN_HEIGHT + fracPartY)/SCREEN_HEIGHT;

	// Draw the background -- a quad that fills the screen and has it's texture
	// coordinates adjusted to appear correct
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glBegin(GL_QUADS);
		glTexCoord2f(texX, texY);
		glVertex2f(0, 0);
		glTexCoord2f(texX, texY+1);
		glVertex2f(0, SCREEN_HEIGHT);
		glTexCoord2f(texX+1, texY+1);
		glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);
		glTexCoord2f(texX+1, texY);
		glVertex2f(SCREEN_WIDTH, 0);
	glEnd();

	craft->Draw();
	glTranslatef(-craft->x+evilcraft->x, -craft->y+evilcraft->y, 0);
	evilcraft->Draw();
	glPopMatrix();

	glutSwapBuffers();
} // end function display()

int lastMvmtUpdTime;
void idle(void) {
	int time = glutGet(GLUT_ELAPSED_TIME);
	int delta = time - lastTime;
	lastTime = time;

	craft->Update(delta);
	evilcraft->Update(delta);

	if(lastMvmtUpdTime + 15 < glutGet(GLUT_ELAPSED_TIME)) {
		lastMvmtUpdTime = glutGet(GLUT_ELAPSED_TIME);
		float mvmtFloats[] = { craft->x, craft->y, craft->xVel, craft->yVel , craft->angle, craft->angularVel };
		cout << sizeof(float);
		sendMessage((char*)mvmtFloats, 6*sizeof(float), MSG_MVMT_UPD);
	}

	glutPostRedisplay();
}

void updateEvilCraft(char * mvmtBytes) {
	float * mvmtFloats = (float *)mvmtBytes;
	evilcraft->x 		= mvmtFloats[0];
	evilcraft->y 		= mvmtFloats[1];
	evilcraft->xVel		= mvmtFloats[2];
	evilcraft->yVel		= mvmtFloats[3];
	evilcraft->angle 	= mvmtFloats[4];
	evilcraft->angularVel 	= mvmtFloats[5];
} // end updateEvilCraft()

void keyDown(unsigned char key, int, int) {
	switch (key) {
		case 27:
			stopNetwork();
			exit(0);
			break;
		case 'w':
			craft->BeginThrust();
			break;
		case ' ':
			craft->BeginTurboThrust();
			break;
		case 'a':
			craft->BeginRotatingCW();
			break;
		case 'd':
			craft->BeginRotatingCCW();
			break;
	}
} // end function keyDown()

void keyUp(unsigned char key, int, int) {
	switch (key) {
		case 'w':
			craft->EndThrust();
			break;
		case ' ':
			craft->EndTurboThrust();
			break;
		case 'a':
			craft->EndRotatingCW();
			break;
		case 'd':
			craft->EndRotatingCCW();
			break;
	} // end switch
} // end function keyUp()

void specialDown(int key, int, int) {
  switch (key) {
  } // end switch()
} // end function specialDown()

void specialUp(int key, int, int) {
  switch (key) {
  } // end switch()
} // end function specialUp()

void loadTGATexture(const char * filename, int index, bool clamp) {
	LTGA * l = new LTGA(filename);
	if(l == 0)
	{
		printf("Error loading texture\n");
		exit(0);
	}

	glBindTexture(GL_TEXTURE_2D, textures[index]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if(clamp) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	// Determine whether the image has transparency or not and load accordingly
	GLint format = (l->GetAlphaDepth()) ? GL_RGBA : GL_RGB;
	glTexImage2D (GL_TEXTURE_2D,
		      0,
		      format,
		      l->GetImageWidth(),
		      l->GetImageHeight(),
		      0,
		      format,
		      GL_UNSIGNED_BYTE,
		      l->GetPixels() );
	delete l;	
} // end loadTGATexture()

void initWindow(void) {
  glutIgnoreKeyRepeat(1);

  glutDisplayFunc(display);
  glutKeyboardFunc(keyDown);
  glutKeyboardUpFunc(keyUp);
  glutSpecialFunc(specialDown);
  glutSpecialUpFunc(specialUp);
  glutIdleFunc(idle);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
} // end function initWindow()

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutCreateWindow("Bongai-O");
	glutReshapeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);

	initWindow();

	textures = new GLuint[NUM_TEXTURES];
	glGenTextures(NUM_TEXTURES, textures);
	loadTGATexture("orionnebula.tga", BG_TEX, false);
	loadTGATexture("ship.tga", SHIP_TEX, true);
	loadTGATexture("rocketlauncher.tga", ROCK_LAUNCH_TEX, true);

	craft = new SpaceCraft();
	evilcraft = new SpaceCraft();

	lastTime = 0;
	lastMvmtUpdTime = glutGet(GLUT_ELAPSED_TIME);

	startNetwork("127.0.0.1");

	glutMainLoop();
	return 0; 
} // end function main
