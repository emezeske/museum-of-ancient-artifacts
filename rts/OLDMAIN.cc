#include <cstdlib>
#include <cstring>
#include <iostream>
#include <math.h>
#include <string>
#include <GL/glut.h>
#include "ltga.h"
#include "textdisplayer.h"
using namespace std;

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

int lastTime;
int currentWindow;

const int hmWidth = 32;
const GLdouble hmSpacing = 1.0;
GLdouble heightMap[hmWidth][hmWidth];
GLdouble sphereX, sphereY;
GLfloat angle;
float rotationRate;
GLuint * heightMapTexture;
bool inWireframeMode;
float sphereRadius;

struct Point3D {
	GLdouble x, y, z;
};

struct PlaneEq {
	GLdouble x, y, z, c;
};

PlaneEq getNormal(GLdouble x, GLdouble y) {
	// Find three points that lay on the plane in question
	Point3D p1, p2, p3;
	x += (GLdouble)hmWidth / 2 * hmSpacing;
	y += (GLdouble)hmWidth / 2 * hmSpacing;

	p1.x = floor(x); 
	p1.y = floor(y);
	p2.x = p1.x + 1;
	p2.y = p1.y;
	p3.x = p1.x;
	p3.y = p1.y + 1;

	GLdouble tempX = x - floor(p1.x);
	GLdouble tempY = y - floor(p1.y);

	// Choose the correct triangle for any given square
	if(tempX+tempY > hmSpacing) 
	{
		++p1.x;
		++p1.y;
	}

	p1.z = heightMap[(int)p1.x][(int)p1.y];
	p2.z = heightMap[(int)p2.x][(int)p2.y];
	p3.z = heightMap[(int)p3.x][(int)p3.y];

	// Find the two vectors that make up the angle 2 1 3
	Point3D vec1, vec2;

	vec1.x = p2.x - p1.x;
	vec1.y = p2.y - p1.y;
	vec1.z = p2.z - p1.z;
	vec2.x = p3.x - p1.x;
	vec2.y = p3.y - p1.y;
	vec2.z = p3.z - p1.z;

	// Find the cross product of vec1 and vec2 to get the vector that is normal to the plane
	PlaneEq normal;
	normal.x = vec1.y * vec2.z - vec1.z * vec2.y;
	normal.y = vec1.z * vec2.x - vec1.x * vec2.z;
	normal.z = vec1.x * vec2.y - vec1.y * vec2.x;
	
	// Normalize normal
	float normalLen = sqrt(pow(normal.x, 2) + pow(normal.y, 2) + pow(normal.z, 2));
	normal.x = x / normalLen;
	normal.y = y / normalLen;
	normal.z = z / normalLen;

	// (normal.x)X + (normal.y)Y + (normal.z)Z + c = 0 is the plane's equation. Find c by 
	// substituting the values from p1 in.
	normal.c = -(normal.x * p1.x + normal.y * p1.y + normal.z * p1.z);

	return normal;
}

void getGravity(PlaneEq normal) {

	// X product normal and Gravity vectors, and then X product resulting vector with normal 
	// again to get the force on the object 
	Point3D force, grav;
	grav.x = 0;
	grav.y = 0;
	grav.z = 0.01;

	force.x = normal.y * grav.z - normal.z * grav.y;
	force.y = normal.z * grav.x - normal.x * grav.z;
	force.z = normal.x * grav.y - normal.y * grav.x;
}

GLdouble getHeight(PlaneEq normal) {

	// Determine z (height) when x and y are plugged in and return!
	// normal.z should not ever be 0...
	return -(normal.x * x + normal.y * y + c) / normal.z;
}

void raiseTerrain(const float heightChange) {
	
	const int rectSideLen = (int)(2 * sphereRadius);
	int rectX = (int)floor((float)sphereX + (float)hmWidth / 2 * hmSpacing - sphereRadius);
	int rectY = (int)floor((float)sphereY + (float)hmWidth / 2 * hmSpacing - sphereRadius);
	for(int x = rectX; x < rectX + rectSideLen; ++x)
		for(int y = rectY; y < rectY + rectSideLen; ++y)
		{
			printf("checking x = %d, y = %d\n", x, y);
			if((pow((float)x - sphereX, 2.0) + pow((float)y - sphereY, 2)) < pow(sphereRadius, 2) )
				heightMap[x][y] += heightChange;
		}
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	if(inWireframeMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_TEXTURE_2D);
	}

	// Draw the height-mapped terrain
	glPushMatrix();
	glRotatef(angle, 0.0, 0.0, 1.0);
	glTranslatef(-(hmWidth / 2 * hmSpacing), -(hmWidth / 2 * hmSpacing), 0.0);

	glBindTexture(GL_TEXTURE_2D, *heightMapTexture);
	glColor3f(1.0, 1.0, 1.0);

	for(int y = 0; y < hmWidth - 1; ++y) {
		glBegin(GL_TRIANGLE_STRIP);

		for(int x = 0; x < hmWidth; ++x) {
			glColor3f(abs(heightMap[x][y]), abs(heightMap[x][y]), abs(heightMap[x][y]));
			GLdouble texX = (float)x / (float)hmWidth * 10;
			texX -= floor(texX);
			GLdouble texY = (float)y / (float)hmWidth * 10;
			texY -= floor(texY);

			glTexCoord2f(texX, texY);
			glVertex3f(x * hmSpacing, y * hmSpacing, heightMap[x][y]);

			glColor3f(abs(heightMap[x][y+1]), abs(heightMap[x][y+1]), abs(heightMap[x][y+1]));
			texY = (float)(y+1) / (float)hmWidth * 10;
			texY -= floor(texY);
			glTexCoord2f(texX, texY);
			glVertex3f(x * hmSpacing, (y + 1) * hmSpacing, heightMap[x][y+1]);
		}
		glEnd();
	}
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Draw the testisphere
	glPushMatrix();
	glRotatef(angle, 0.0, 0.0, 1.0);
	glTranslatef(sphereX, sphereY, heightAt(sphereX, sphereY) - 0.5);
	glColor4f(0.3, 1.0, 0.3, 0.4);
	glutSolidCone(0.21, 0.5, 100, 100);
	glRotatef(90, 0.0, 1.0, 0.0);
	glutWireSphere(sphereRadius, 2, 10);
	glPopMatrix();

	// Try to draw the UI
  	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT, -1.0, 1.0);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	fontSet(FONT_SIZE, 15.0);
	string tempString = "wasd: move circle     er: rotate camera      1: toggle wireframe        jk: raise or lower terrain     ui: change radius of circle";
	drawTextBox(tempString, 0.0, 25.0, 800.0);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glutSwapBuffers();
} // end function display()

void idle(void) {
	int time = glutGet(GLUT_ELAPSED_TIME);
	int delta = time - lastTime;
	lastTime = time;

	angle += rotationRate * delta;

	glutPostWindowRedisplay(currentWindow);
}

void keyDown(unsigned char key, int, int) {
	switch (key) {
		case 27:
			exit(0);
			break;
		case 'w':
			sphereY += 0.1;
			break;
		case 'a':
			sphereX += 0.1;
			break;
		case 's':
			sphereY -= 0.1;
			break;
		case 'd':
			sphereX -= 0.1;
			break;
		case 'e':
			rotationRate = -0.07;
			break;
		case 'r':
			rotationRate = 0.07;
			break;
		case 'u':
			sphereRadius -= 0.1;
			break;
		case 'i':
			sphereRadius += 0.1;
			break;
		case 'j':
			raiseTerrain(-0.1);
			break;
		case 'k':
			raiseTerrain(0.1);
			break;
		case '1':
			inWireframeMode = !inWireframeMode;
			break;
	}
} // end function keyDown()

void keyUp(unsigned char key, int, int) {
  switch (key) {
		case 'e':
			rotationRate = 0.0;
			break;
		case 'r':
			rotationRate = 0.0;
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

void loadTextures(void) {
	heightMapTexture = new GLuint;
	LTGA *l;

	glGenTextures(1, heightMapTexture);

	l = new LTGA("./gfx/grass.tga");
	if(l == 0)
	{
		printf("Error loading texture\n");
		exit(0);
	}

	glBindTexture(GL_TEXTURE_2D, *heightMapTexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glTexImage2D (GL_TEXTURE_2D,
		      0,
		      GL_RGB,
		      l->GetImageWidth(),
		      l->GetImageHeight(),
		      0,
		      GL_RGB,
		      GL_UNSIGNED_BYTE,
		      l->GetPixels() );

	delete l;	
} // end loadTextures()

void initWindow(void) {
  glutIgnoreKeyRepeat(0);

  glutDisplayFunc(display);
  glutKeyboardFunc(keyDown);
  glutKeyboardUpFunc(keyUp);
  glutSpecialFunc(specialDown);
  glutSpecialUpFunc(specialUp);
  glutIdleFunc(idle);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (GLdouble)SCREEN_WIDTH/(GLdouble)SCREEN_HEIGHT, 1, 100);
  gluLookAt(0.0, -20.0, -5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  //currentWindow = glutGetWindow();
} // end function initWindow()

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	//glutGameModeString("1024x768:32@60");
	//glutEnterGameMode();
	currentWindow = glutCreateWindow("Bamboozlification");
	glutReshapeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);

	initTextDisplayer();
	initWindow();
	loadTextures();
	angle = 0.0;
	sphereX = sphereY = 0;
	lastTime = 0;
	rotationRate = 0;
	inWireframeMode = false;
	sphereRadius = 0.5;

	for(int x = 0; x < hmWidth; ++x)
		for(int y = 0; y < hmWidth; ++y)
			heightMap[x][y] = -1 * (float)(rand() % 255) / 255.0;

	glutMainLoop();
	return 0; 
} // end function main


