#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>
#include <math.h>
#include "ltga.h"
using namespace std;

GLuint screenTex;

const int MAX_ITERS = 1000;

const int SCREEN_W = 512;
const int SCREEN_H = 512;

double 	PLANE_R	  = -3.0,
    	PLANE_I   = -3.0,
 	PLANE_R_W =  6.0,
    	PLANE_I_H =  6.0;

double ORIG_R	= PLANE_R,
       ORIG_I	= PLANE_I,
       ORIG_W	= PLANE_R_W,
       ORIG_H	= PLANE_I_H;

double 	SCREEN_PLANE_RATIO_X = 1/(double)SCREEN_W*PLANE_R_W,
	SCREEN_PLANE_RATIO_Y = 1/(double)SCREEN_H*PLANE_I_H;

struct RGBClr {
	char r, g, b;
}; // end struct RGBClr

unsigned char pixels[SCREEN_W][SCREEN_H][3];
unsigned char im_pixels[SCREEN_W][SCREEN_H][3];

void loadImagePixels() {
	LTGA *l = new LTGA("image.tga");
	unsigned char * t_bytes = l->GetPixels();
	for(int x = SCREEN_W-1; x >= 0; --x)
		for(int y = 0; y < SCREEN_H; ++y)
		{
			im_pixels[x][SCREEN_H - y][0] = t_bytes[3*(x+y*SCREEN_W)+0];
			im_pixels[x][SCREEN_H - y][1] = t_bytes[3*(x+y*SCREEN_W)+1];
			im_pixels[x][SCREEN_H - y][2] = t_bytes[3*(x+y*SCREEN_W)+2];
		}
} // end loadImagePixels()

void buildFractal(void) {
	for(int x=0; x<SCREEN_W; ++x)
		for(int y=0; y<SCREEN_H; ++y) {
			double zr, zi, cr, ci, zrsq, zisq, tr, ti;
			zr = cr = PLANE_R + x*SCREEN_PLANE_RATIO_X; 
                        zi = ci = PLANE_I + y*SCREEN_PLANE_RATIO_X;
			bool trapped = false;
			for(int i=0; i<MAX_ITERS; ++i) {
				double 	zrsq = zr*zr,
					zisq = zi*zi,
					zizr = zi*zr,
					zizrt2 = zizr + zizr;
				zr = zrsq - zisq + cr;
				zi = zizrt2 + ci;
				if(zrsq + zisq > 4.0)
					break;
				int tx, ty;
				tx = (int)((zr - ORIG_R)/ORIG_W*SCREEN_W);
				ty = (int)((zi - ORIG_I)/ORIG_H*SCREEN_H);
				if( (tx < 0) || (tx >= SCREEN_W) || (ty < 0) || (ty >= SCREEN_H) )
					continue;
				if(!trapped)
				if(im_pixels[tx][ty][0] != 255)
				if(im_pixels[tx][ty][1] != 255)
				if(im_pixels[tx][ty][2] != 255) {
					tr = zr;
					ti = zi;
					trapped = true;
				}
			}
			if(trapped == true) {
				int tx, ty;
				tx = (int)((tr - ORIG_R)/ORIG_W*SCREEN_W);
				ty = (int)((ti - ORIG_I)/ORIG_H*SCREEN_H);
				pixels[x][y][0] = im_pixels[tx][ty][0];
				pixels[x][y][1] = im_pixels[tx][ty][1];
				pixels[x][y][2] = im_pixels[tx][ty][2];
			} else {
				pixels[x][y][0] = ~0xcc;
				pixels[x][y][1] = ~0x41;
				pixels[x][y][2] = ~0x3c;
			}
		}
} // end buildFractal()

double lastMouseR, lastMouseI;
void glutMouseDown(int button, int isUp, int x, int y) {
	y = SCREEN_H - y;
	if(!isUp) {
		lastMouseR = PLANE_R + (double)x/(double)SCREEN_W*PLANE_R_W;
		lastMouseI = PLANE_I + (double)y/(double)SCREEN_H*PLANE_I_H;
	} else {
		double r1, i1, r2, i2;
		r1 = lastMouseR;
		i1 = lastMouseI;
		r2 = PLANE_R + (double)x/(double)SCREEN_W*PLANE_R_W;
		i2 = PLANE_I + (double)y/(double)SCREEN_H*PLANE_I_H;
		PLANE_R = (r1 < r2) ? r1 : r2;
		PLANE_R_W = fabs(r2 - r1);
		PLANE_I = (i1 < i2) ? i1 : i2;
		PLANE_I_H = fabs(i2 - i1);
		SCREEN_PLANE_RATIO_X = 1/(double)SCREEN_W*PLANE_R_W,
		SCREEN_PLANE_RATIO_Y = 1/(double)SCREEN_H*PLANE_I_H;
	}
} // end glutMouseDown()

void idle(void) {
	buildFractal();
	//Bind pixel image to texture
	glTexImage2D (GL_TEXTURE_2D,
		      0,
		      GL_RGB,
		      SCREEN_W,
		      SCREEN_H,
		      0,
		      GL_RGB,
		      GL_UNSIGNED_BYTE,
		      pixels);
	// Redraw
	glutPostRedisplay();
} // end function idle()

void display(void) {
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 	0.0);
		glVertex2f(  0.0,  	0.0);
		glTexCoord2f(1.0, 	0.0);
		glVertex2f(  0.0,	SCREEN_H);
		glTexCoord2f(1.0, 	1.0);
		glVertex2f(SCREEN_W, 	SCREEN_H);
		glTexCoord2f(0.0, 	1.0);
		glVertex2f(SCREEN_W,  	0.0);
	glEnd();
	glutSwapBuffers();
} // end function display()

void keyboardDown(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			exit(0);
			break;
		case 'p':
			printf("%f, %f, %f, %f\n", PLANE_R, PLANE_I, PLANE_R_W, PLANE_I_H);
	} // end switch(key)
} // end function keyboardDown()

int main(int argc, char ** argv) {
	// Get glut started
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//	glutGameModeString("1024x768:32@60");
//	glutEnterGameMode();
	glutCreateWindow("frac");
	glutReshapeWindow(SCREEN_W, SCREEN_H);

	// Set up glut
	glutIgnoreKeyRepeat(0);
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardDown);
	glutMouseFunc(glutMouseDown);

	// Set up opengl
	gluOrtho2D(0, SCREEN_W, 0, SCREEN_H);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Set up the texture environment
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glGenTextures(0, &screenTex);

	loadImagePixels();
	glutMainLoop();
	return 0;
} // end function main()
