#include <GL/glut.h>
#include <cstdlib>
#include <vector>
#include <float.h>
#include <math.h>

#include "vector.h"
#include "ltga.h"
#include "depthmap.h"
#include "utils.h"

LTGA *pattern;
GLuint screenTex;
Plane 		screenPlane; 
Depthmap *depthmap;

const int screenW		= 512;
const int screenH		= 512;
float distFromEyeToScreen 	= 2530;
float distBetweenEyes 		= 1500;
bool needToRedraw = true;
unsigned char pixels[screenH][screenW][3];

// LEFT EYE position
Vector3 LEPosition;
// RIGHT EYE position
Vector3 REPosition;
	
void turnArrayIntoTexture(int width, int height, unsigned char *bytes, GLuint texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D (GL_TEXTURE_2D,
		      0,
		      GL_RGB,
		      width,
		      height,
		      0,
		      GL_RGB,
		      GL_UNSIGNED_BYTE,
		      bytes);
} // end turnArrayIntoTexture()

void createStereogram() {
	// DEBUG
	//memset(&pixels, 0, screenW*screenH*3);
	// Take the pattern image and copy to the screen image
	int imageWidth = pattern->GetImageWidth();
	int imageHeight = pattern->GetImageHeight();
	unsigned char *patternBytes = (unsigned char *)pattern->GetPixels();
	for(int x = imageWidth-1; x >= 0; --x)
		for(int y = 0; y < imageHeight; ++y)
		{
			pixels[x][imageHeight - y][0] = patternBytes[3*(x+y*imageWidth)+0];
			pixels[x][imageHeight - y][1] = patternBytes[3*(x+y*imageWidth)+1];
			pixels[x][imageHeight - y][2] = patternBytes[3*(x+y*imageWidth)+2];
		}
	// create a ray from the right eye through (x, y) on the screen plane 
	// and interpolate it's intersection with the depthmap, then find the ray
	// that passes from the left eye to that point on the depthmap and interpolate
	// the color of the pixels that the left eye ray passes through and slap that
	// color on (x, y) for all pixels
	for(int curY=0; curY<screenH; ++curY)
	{
		Vector3 LEPosition(	screenW*0.5 - distBetweenEyes*0.5, 
					curY, 
					distFromEyeToScreen);
		Vector3 REPosition(	screenW*0.5 + distBetweenEyes*0.5, 
					curY, 
					distFromEyeToScreen);
		for(int curX=imageWidth; curX<screenW; ++curX)
		{
			Vector3 REDirection 	 = REPosition - Vector3(curX, curY, 0),
				hMIntersection	 = depthmap->RayIntersection(REDirection, REPosition),
				LEDirection 	 = LEPosition - hMIntersection,
				LEScreenI	 = planeRayIntersect(screenPlane, LEDirection, LEPosition);
			// Keep program from segfaulting if LEScreenI is offscreen
			if(LEScreenI.x < 0 || LEScreenI.x >= screenW - 1)
				continue;
			unsigned char r1 = pixels[(int)LEScreenI.x][(int)LEScreenI.y][0],
			              g1 = pixels[(int)LEScreenI.x][(int)LEScreenI.y][1],
			              b1 = pixels[(int)LEScreenI.x][(int)LEScreenI.y][2],
				      r2 = pixels[(int)ceil(LEScreenI.x)][(int)LEScreenI.y][0],
			              g2 = pixels[(int)ceil(LEScreenI.x)][(int)LEScreenI.y][1],
			              b2 = pixels[(int)ceil(LEScreenI.x)][(int)LEScreenI.y][2];
			float	leftWeight = LEScreenI.x - floor(LEScreenI.x),
				rightWeight;
			if(leftWeight <= 0.5)
			{
				rightWeight = 1.0 - leftWeight;
			} else {
				rightWeight = leftWeight;
				leftWeight = 1.0 - rightWeight;
			}
			unsigned char 	r = (unsigned char)(leftWeight*r1 + rightWeight*r2),
					g = (unsigned char)(leftWeight*g1 + rightWeight*g2),
					b = (unsigned char)(leftWeight*b1 + rightWeight*b2);
			pixels[curX][curY][0] = r;
			pixels[curX][curY][1] = g;
			pixels[curX][curY][2] = b;
		} // end for
	} // end outer for
	turnArrayIntoTexture(screenW, screenH, (unsigned char *)pixels, screenTex);
} // end function createStereogram()

void idle(void) {
	if(needToRedraw)
	{
		createStereogram();
		needToRedraw = false;
		glutPostRedisplay();
	}
} // end function idle()

void display(void) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, screenTex);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0, 	0.0);
		glVertex2f(  0.0,  	0.0);
		glTexCoord2f(0.0, 	0.0);
		glVertex2f(  0.0,	screenH);
		glTexCoord2f(0.0, 	1.0);
		glVertex2f(screenW, 	screenH);
		glTexCoord2f(1.0, 	1.0);
		glVertex2f(screenW,  	0.0);
	glEnd();
		
	glutSwapBuffers();
} // end function display()

void keyboardDown(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			exit(0);
			break;
		case 's':
			distFromEyeToScreen+=10;
			needToRedraw = true;
			break;
		case 'd':
			distBetweenEyes+=10;
			needToRedraw = true;
			break;
		case 'x':
			distFromEyeToScreen-=10;
			needToRedraw = true;
			break;
		case 'c':
			distBetweenEyes-=10;
			needToRedraw = true;
			break;
		case 'p':
			printf("distFromEyeToScreen: %f\ndistBetweenEyes: %f\n", distFromEyeToScreen, distBetweenEyes);
			break;
	} // end switch(key)
} // end function keyboardDown()

int main(int argc, char ** argv) {
	// Get glut started
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Stereogame");
	glutReshapeWindow(screenW, screenH);

	// Set up glut
	glutIgnoreKeyRepeat(0);
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardDown);

	// Set up opengl
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screenW, 0, screenH, 0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Set up the texture environment
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glGenTextures(0, &screenTex);

	// Set the screenPlane's values appropriately
	screenPlane.a = screenPlane.b = screenPlane.d = 0.0;
	screenPlane.c = 1.0;

	// Set up the pattern image
	pattern  = new LTGA("./pattern.tga");
	if(pattern == 0)
		bailout("Failed to open pattern tga...\n");
	// Build Depthmap
	depthmap = new Depthmap("./depthmap.tga");

	// GO! -- (not the game)
	glutMainLoop();
	return 0;
} // end function main()
