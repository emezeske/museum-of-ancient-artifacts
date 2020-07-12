#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>
#include <math.h>
#include "libtarga.h"
using namespace std;

GLuint screenTex;

enum ColoringMethod { TRAP_RADIUS, NUM_ITERS, TRAP_ANGLE };

ColoringMethod colorMeth = TRAP_RADIUS;

int MAX_ITERS = 32;

const int SCREEN_W = 512;
const int SCREEN_H = 512;

int NUM_SAMPLES = 1;
double COLOR_CONV = (double)255/((double)NUM_SAMPLES*(double)NUM_SAMPLES);
double SP_OVERSAMPLING = 0.000;

double 	PLANE_R	  = -2.3,
    	PLANE_I   = -1.4,
 	PLANE_R_W =  4.0,
    	PLANE_I_H =  3.0;

double 	SCREEN_PLANE_RATIO_X = 1/(double)SCREEN_W*PLANE_R_W,
	SCREEN_PLANE_RATIO_Y = 1/(double)SCREEN_H*PLANE_I_H;

bool changed = true;
bool animatePalette = false;
double pal_offset = 0.0;

unsigned char pixels[SCREEN_W][SCREEN_H][3];

double 	px_pal_num[SCREEN_W][SCREEN_H];

struct FRGBClr {
	double r, g, b;
}; 

const double NOT_TRAPPED = 99999;

double getTrapRadSq(double cr, double ci) {
	double zr, zi;
	zr = cr; 
        zi = ci;
	double radiussq = NOT_TRAPPED;
	for(int i=0; i<MAX_ITERS; ++i) {
		double 	zrsq = zr*zr,
			zisq = zi*zi,
			zizr = zi*zr,
			zizrt2 = zizr + zizr;
		zr = zrsq - zisq + cr;
		zi = zizrt2 + ci;
		double zrsqzisq = zrsq + zisq;
		if(zrsqzisq < 1.0) {
			radiussq  = zrsqzisq;
		} else if(zrsqzisq > 4.0)
			break;
	}
	return radiussq;
} // end getTrapRadSq()

double getTrapAngle(double cr, double ci) {
	double zr, zi, tr, ti;
	bool trapped = false;
	zr = cr; 
        zi = ci;
	for(int i=0; i<MAX_ITERS; ++i) {
		double 	zrsq = zr*zr,
			zisq = zi*zi,
			zizr = zi*zr,
			zizrt2 = zizr + zizr;
		zr = zrsq - zisq + cr;
		zi = zizrt2 + ci;
		double zrsqzisq = zrsq + zisq;
		if(zrsqzisq < 1.0) {
			trapped = true;
			tr = zr;
			ti = zi;
		} else if(zrsqzisq > 4.0)
			break;
	}
	if(trapped)
		return atan2(ti, tr);
	else
		return NOT_TRAPPED;
} // end getTrapRadSq()

int getNumIters(double cr, double ci) {
	double zr, zi;
	zr = cr; 
        zi = ci;
	int i=0;
	for(; i<MAX_ITERS; ++i) {
		double 	zrsq = zr*zr,
			zisq = zi*zi,
			zizr = zi*zr,
			zizrt2 = zizr + zizr;
		zr = zrsq - zisq + cr;
		zi = zizrt2 + ci;
		if(zrsq+zisq > 4.0)
			break;
	}
	return i;
} // end getColorAt()

void buildFractal(void) {
	FRGBClr t;
	double totr, totg, totb;
	double lower_bound = 0.0 - SP_OVERSAMPLING,
	       upper_bound = 1.0 + SP_OVERSAMPLING,
	       total_bounds = 1.0 + 2.0 * SP_OVERSAMPLING;
	double cr, ci;
	unsigned char *px = new (unsigned char)[SCREEN_W*SCREEN_H*3];
	for(int x=0; x<SCREEN_W; ++x)
		for(int y=0; y<SCREEN_H; ++y) {
			totr = totg = totb = 0;
			for(double fx=lower_bound; fx<upper_bound; fx+=total_bounds/(double)NUM_SAMPLES)
				for(double fy=lower_bound; fy<upper_bound; fy+=total_bounds/(double)NUM_SAMPLES) {
					cr = PLANE_R + ((double)x+fx)*SCREEN_PLANE_RATIO_X; 
        				ci = PLANE_I + ((double)y+fy)*SCREEN_PLANE_RATIO_Y;
					switch(colorMeth) {
						case TRAP_RADIUS:
							{
								double rsq = getTrapRadSq(cr, ci);
								if(rsq != NOT_TRAPPED) {
									totr += fabs(sin(rsq*4.1 + pal_offset));
									totg += 0;//fabs(sin(rsq + pal_offset));
									totb += fabs(sin(rsq*1.5 + pal_offset));
								} else {
									totr += 0;
									totg += 0;
									totb += 0;
								}
								break;
							}
						case NUM_ITERS:
							{
								int i = getNumIters(cr, ci);
								if(i < MAX_ITERS-1) {
									totr += fabs(sin(0.032*M_PI*i + pal_offset));
									totg += 0;//fabs(sin(15*radiussq + pal_offset));
									totb += fabs(cos(0.0555*M_PI*i + pal_offset));
								} else {
									totr += 0;
									totg += 0;
									totb += 0;
								}
								break;
							}
						case TRAP_ANGLE:
							{
								double ang = getTrapAngle(cr,ci);
								if(ang != NOT_TRAPPED) {
									totr += fabs(sin(4*ang + pal_offset));
									totg += 0;//fabs(sin(rsq*rsq + pal_offset));
									totb += fabs(sin(7*ang + pal_offset));
								} else {
									totr += 0;
									totg += 0;
									totb += 0;
								}
								break;
							}
					} // end switch
				}

			px[y*(SCREEN_W*3)+x*3+0] = (unsigned char)((totr)*COLOR_CONV);
			px[y*(SCREEN_W*3)+x*3+1] = (unsigned char)((totg)*COLOR_CONV);
			px[y*(SCREEN_W*3)+x*3+2] = (unsigned char)((totb)*COLOR_CONV);
		}
			for(int x=0; x<SCREEN_W; ++x)
				for(int y=0; y<SCREEN_H; ++y) {
					pixels[x][y][0] = px[y*(SCREEN_W*3)+x*3+0];
					pixels[x][y][1] = px[y*(SCREEN_W*3)+x*3+1];
					pixels[x][y][2] = px[y*(SCREEN_W*3)+x*3+2];
				}
} // end buildFractal()

double lastMouseR, lastMouseI;
int lastMouseX, lastMouseY, curMouseX, curMouseY;
bool mouseIsDown = false;

void glutMouseDown(int button, int isUp, int x, int y) {
	y = SCREEN_H - y;
	if(!isUp) {
		mouseIsDown = true;
		lastMouseR = PLANE_R + (double)x/(double)SCREEN_W*PLANE_R_W;
		lastMouseI = PLANE_I + (double)y/(double)SCREEN_H*PLANE_I_H;
		curMouseX = lastMouseX = x;
		curMouseY = lastMouseY = y;
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
		changed = true;
		mouseIsDown = false;
	}
} // end glutMouseDown()

void glutMouseMove(int x, int y) {
	curMouseX = x;
	curMouseY = SCREEN_H - y;
} // end glutMouseMove()

void idle(void) {
	if(animatePalette) {
		pal_offset += 0.10;
		changed = true;
	}
	if(changed) {
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
		changed = false;
	}
	// Redraw
	glutPostRedisplay();
} // end function idle()

void display(void) {
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 	0.0);
		glVertex2f(  0.0,  	0.0);
		glTexCoord2f(1.0, 	0.0);
		glVertex2f(  0.0,	SCREEN_H+256);
		glTexCoord2f(1.0, 	1.0);
		glVertex2f(SCREEN_W, 	SCREEN_H+256);
		glTexCoord2f(0.0, 	1.0);
		glVertex2f(SCREEN_W,  	0.0);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
	if(mouseIsDown) {
		glBegin(GL_QUADS);
			glColor4f(0.0, 0.0, 1.0, 0.5);
			glVertex2f(lastMouseX, lastMouseY);
			glColor4f(0.0, 1.0, 0.0, 0.5);
			glVertex2f(lastMouseX, curMouseY);
			glColor4f(1.0, 0.0, 0.0, 0.5);
			glVertex2f(curMouseX,  curMouseY);
			glColor4f(1.0, 0.0, 1.0, 0.5);
			glVertex2f(curMouseX,  lastMouseY);
		glEnd();
	} 
	glutSwapBuffers();
} // end function display()

void keyboardDown(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			exit(0);
			break;
		case 'r':
		 	PLANE_R	  = -2.3,
		    	PLANE_I   = -1.4,
		 	PLANE_R_W =  4.0,
		    	PLANE_I_H =  3.0;
			SCREEN_PLANE_RATIO_X = 1/(double)SCREEN_W*PLANE_R_W,
			SCREEN_PLANE_RATIO_Y = 1/(double)SCREEN_H*PLANE_I_H;
			changed = true;
			break;
		case 'a':
			animatePalette = !animatePalette;
			break;
		case 'c':
			colorMeth = NUM_ITERS;
			changed = true;
			break;
		case 'v':
			colorMeth = TRAP_RADIUS;
			changed = true;
			break;
		case 'b':
			colorMeth = TRAP_ANGLE;
			changed = true;
			break;
		case 'i':
			NUM_SAMPLES++;
			COLOR_CONV = (double)255/((double)NUM_SAMPLES*(double)NUM_SAMPLES);
			changed = true;
			break;
		case 'k':
			NUM_SAMPLES--;
			if(NUM_SAMPLES < 1)
				NUM_SAMPLES = 1;
			COLOR_CONV = (double)255/((double)NUM_SAMPLES*(double)NUM_SAMPLES);
			changed = true;
			break;
		case 'o':
			MAX_ITERS += 32;
			changed = true;
			break;
		case 'l':
			MAX_ITERS -= 32;
			if(MAX_ITERS < 32)
				MAX_ITERS = 32;
			changed = true;
			break;
		case 's':
			unsigned char tga_pixels[SCREEN_W][SCREEN_H+256][3];
			for(int x=0; x<SCREEN_H; ++x)
				for(int y=0; y<SCREEN_W; ++y)
					for(int i=0; i<3; ++i)
						tga_pixels[x][y][i] = pixels[y][x][i];
			tga_write_raw("image.tga", 1024, 768, (unsigned char*)tga_pixels, TGA_TRUECOLOR_24);
			break;
	} // end switch(key)
} // end function keyboardDown()

int main(int argc, char ** argv) {
	// Get glut started
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow( "wacky" );
	glutReshapeWindow( 1024, 768 );
	//glutGameModeString("1024x768:32@60");
	//glutEnterGameMode();

	// Set up glut
	glutIgnoreKeyRepeat(0);
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardDown);
	glutMouseFunc(glutMouseDown);
	glutMotionFunc(glutMouseMove);

  	glEnable(GL_BLEND);
  	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set up opengl
	gluOrtho2D(0, SCREEN_W, 0, SCREEN_H);

	// Set up the texture environment
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenTextures(0, &screenTex);

	glutMainLoop();
	return 0;
} // end function main()
