#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>
#include <math.h>
using namespace std;

GLuint screenTex;

const int MAX_ITERS = 32;

const int SCREEN_W = 512;
const int SCREEN_H = 512;

double 	PLANE_R	  = -3.0,
    	PLANE_I   = -3.0,
 	PLANE_R_W =  6.0,
    	PLANE_I_H =  6.0;

double 	SCREEN_PLANE_RATIO_X = 1/(double)SCREEN_W*PLANE_R_W,
	SCREEN_PLANE_RATIO_Y = 1/(double)SCREEN_H*PLANE_I_H;

struct RGBClr {
	char r, g, b;
}; // end struct RGBClr

RGBClr palette[MAX_ITERS];
unsigned char pixels[SCREEN_W][SCREEN_H][3];

void initPalette(void) {
	for(int i=0; i<MAX_ITERS; ++i) {
		palette[i].r = ~((char)(128 * sin((double)i/8*M_PI+0.5*M_PI) + 128));
		palette[i].g = ~0;//((char)(128 * sin((double)i/14*M_PI+0.5*M_PI) + 128));
		palette[i].b = ~((char)(128 * sin((double)i/6*M_PI) + 128));
	}
} // end initPalette()

void cyclePalette(void) {
	RGBClr t = palette[1];
	for(int i=0; i<MAX_ITERS-1; ++i) {
		palette[i] = palette[i+1];
	}
	palette[MAX_ITERS-1]=t;
} // end cyclePalette()

void buildFractal(void) {
	for(int x=0; x<SCREEN_W; ++x)
		for(int y=0; y<SCREEN_H; ++y) {
			double zr, zi, cr, ci, zrsq, zisq;
			zr = cr = PLANE_R + x*SCREEN_PLANE_RATIO_X; 
                        zi = ci = PLANE_I + y*SCREEN_PLANE_RATIO_X;
			int i=0;
			for(; i<MAX_ITERS; ++i) {
				double 	zrsq = zr*zr,
					zisq = zi*zi,
					zizr = zi*zr,
					zizrt2 = zizr + zizr;
				zr = zrsq - zisq + cr;
				zi = zizrt2 + ci;
				if(zrsq + zisq > 4.0)
					break;
			}
			if(i < MAX_ITERS-1) {
				pixels[x][y][0] = palette[i].r;
				pixels[x][y][1] = palette[i].g;
				pixels[x][y][2] = palette[i].b;
			} else {
				pixels[x][y][0] = 255;
				pixels[x][y][1] = 255;
				pixels[x][y][2] = 255;
			}
		}
} // end buildFractal()

double lastMouseR, lastMouseI;
void glutMouseDown(int button, int isUp, int x, int y) {
	if(!isUp) {
		lastMouseR = PLANE_R + (double)x/(double)SCREEN_W*PLANE_R_W;
		lastMouseI = PLANE_I + (double)y/(double)SCREEN_H*PLANE_I_H;
	} else {
		double r1, i1, r2, i2;
		r1 = lastMouseR;
		i1 = lastMouseI;
		r2 = PLANE_R + (double)x/(double)SCREEN_W*PLANE_R_W;
		i2 = PLANE_I + (double)y/(double)SCREEN_H*PLANE_I_H;
		if(r1 < r2) {
			PLANE_R = r1;
			PLANE_R_W = r2 - r1;
		} else {
			PLANE_R = r2;
			PLANE_R_W = r1 - r2;
		}
		if(i1 < i2) {
			PLANE_I = i1;
			PLANE_I_H = i2 - i1;
		} else {
			PLANE_I = i2;
			PLANE_I_H = i1 - i2;
		}
		SCREEN_PLANE_RATIO_X = 1/(double)SCREEN_W*PLANE_R_W,
		SCREEN_PLANE_RATIO_Y = 1/(double)SCREEN_H*PLANE_I_H;
	}
} // end glutMouseDown()

void idle(void) {
	buildFractal();
	cyclePalette();
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
		glTexCoord2f(1.0, 	0.0);
		glVertex2f(  0.0,  	0.0);
		glTexCoord2f(0.0, 	0.0);
		glVertex2f(  0.0,	SCREEN_H);
		glTexCoord2f(0.0, 	1.0);
		glVertex2f(SCREEN_W, 	SCREEN_H);
		glTexCoord2f(1.0, 	1.0);
		glVertex2f(SCREEN_W,  	0.0);
	glEnd();
	glutSwapBuffers();
} // end function display()

void keyboardDown(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			exit(0);
			break;
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

	initPalette();
	glutMainLoop();
	return 0;
} // end function main()
