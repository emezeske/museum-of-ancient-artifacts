#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>
#include <math.h>
#include <unistd.h>
#include "libtarga.h"
#include "SocketServer.h"
#include "Worker.h"
using namespace std;

#define TRAP_RADIUS	0
#define NUM_ITERS	1
#define TRAP_ANGLE	2
int COLOR_METH = TRAP_RADIUS;

#define ZOOM_IN		0
#define ZOOM_OUT	1
#define ZOOM_NONE	2
#define ZOOM_RATE	0.11

#define MOV_L	0
#define MOV_R	1
#define MOV_U	2
#define MOV_D	3
#define MOV_N	4
#define MOV_RATE 0.05

#define PAL_CYC_RATE 0.5

GLuint screenTex;
SocketServer* sockServ;
vector<Worker> workers;
vector<Worker> slackers;

bool VERBOSE;

int 		MAX_ITERS 	= 32,
 		SAMPLE_BLOCK_W 	= 1,
		lastMouseX,
		lastMouseY,
		curMouseX,
		curMouseY,
		zooming		= ZOOM_NONE,
		movingHoriz	= MOV_N,
		movingVert	= MOV_N;
const int	SCREEN_W	= 1024,
		SCREEN_H	= 768;
double 		SP_OVERSAMPLING	= 0.000,
		SCREEN_ASP_RAT 	= SCREEN_W/SCREEN_H,
 		PLANE_R	  	= -2.3,
    		PLANE_I   	= -1.4,
 		PLANE_R_W 	=  4.0,
    		PLANE_I_H 	=  4.0,
 		PAL_OFFSET 	= 0.0,
		SCREEN_PLANE_RATIO_X = 1/(double)SCREEN_W*PLANE_R_W,
		SCREEN_PLANE_RATIO_Y = 1/(double)SCREEN_H*PLANE_I_H,
		lastMouseR,
		lastMouseI;

bool 	changed 	= true,
	animatePalette	= false,
	leftMouseIsDown	= false;

unsigned char pixels[SCREEN_W][SCREEN_H+256][3];

void newWorker(int c) {
	workers.push_back(Worker(c));
	if(VERBOSE)
		printf("Added new worker, assigning #%d\n", c);
} // end newClient()

void lostWorker(int c) {
	for(int i=0; i<workers.size(); ++i)
		if(workers[i].client_no == c) {
			slackers.push_back(workers[i]);
			workers.erase(workers.begin()+i);
			if(VERBOSE)
				printf("Removed worker #%d\n", c);
			break;
		}
} // end lostClient()

void pastePixelFragment(int posX, int posY, int w, int h, unsigned char* px_data) {
	for(int x=0; x<w; ++x)
		for(int y=0; y<h; ++y) {
			pixels[x+posX][y+posY][0] = px_data[y*3*w+x*3];
			pixels[x+posX][y+posY][1] = px_data[y*3*w+x*3+1];
			pixels[x+posX][y+posY][2] = px_data[y*3*w+x*3+2];
		}
} // end pastePixelFragment()

void buildFractal(int px_y, int px_w, int px_h, double cx, double cy, double cw, double ch) {
	int num_workers = workers.size();
	if(num_workers == 0)
		return;
	// Distribute all the workload
	for(int i=0; i<num_workers; ++i) {
		workers[i].posX				= 0;
		workers[i].posY				= px_y + (px_h/num_workers)*i;
		workers[i].cur_req.max_iters		= MAX_ITERS;
		workers[i].cur_req.color_meth		= COLOR_METH;
		workers[i].cur_req.sample_block_w	= SAMPLE_BLOCK_W;
		workers[i].cur_req.pal_offset		= PAL_OFFSET;
		workers[i].cur_req.sp_oversample	= SP_OVERSAMPLING;
		workers[i].cur_req.pxW			= px_w; 
		workers[i].cur_req.pxH			= px_h/num_workers;
		workers[i].cur_req.cmplxX               = cx;              
		workers[i].cur_req.cmplxY               = cy + (ch/num_workers)*i;
		workers[i].cur_req.cmplxW              	= cw;
		workers[i].cur_req.cmplxH		= ch/num_workers;
		sockServ->SendDataToClient(workers[i].client_no, sizeof(FractalBuildRequest), &workers[i].cur_req);
	}
	if(VERBOSE)
		printf("Requests distributed\n");
	// Receive all the finished work pointers
	IncomingData *ptr;
	for(int i=0; slackers.size()+i < num_workers;) {
		if((ptr = sockServ->GetNextData()) != NULL) {
			for(int z=0; z<workers.size(); ++z)
				if(workers[z].client_no == ptr->client_id) {
					pastePixelFragment(	workers[z].posX,
								workers[z].posY,
								workers[z].cur_req.pxW,
								workers[z].cur_req.pxH,
								(unsigned char*)ptr->data);
					if(VERBOSE)
						printf("Got data back from worker #%d\n", z);
				}
			++i;
			delete ptr;
		}
	}
	bustout:
		while(!slackers.empty()) {
			if(VERBOSE)
				printf("Mid-request client loss! Distributing missing data\n");
			Worker t = *(slackers.end()-1);
			slackers.pop_back();
			buildFractal(	t.posY,
					t.cur_req.pxW,
					t.cur_req.pxH,
					t.cur_req.cmplxX,
					t.cur_req.cmplxY,
					t.cur_req.cmplxW,
					t.cur_req.cmplxH);
		}
	changed = false;
} // end buildFractal()

void specialKeyDown(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_LEFT:
			movingHoriz = MOV_L;
			break;
		case GLUT_KEY_RIGHT:
			movingHoriz = MOV_R;
			break;
		case GLUT_KEY_UP:
			movingVert = MOV_U;
			break;
		case GLUT_KEY_DOWN:
			movingVert = MOV_D;
			break;
	} // end switch
} // end specialKeyDown()

void specialKeyUp(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_LEFT:
		case GLUT_KEY_RIGHT:
			movingHoriz = MOV_N;
			break;
		case GLUT_KEY_UP:
		case GLUT_KEY_DOWN:
			movingVert = MOV_N;
			break;
	} // end switch
} // end specialKeyDown()

void keyboardDown(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			sockServ->StopListening();
			delete sockServ;
			exit(0);
			break;
		case 'r':
		 	PLANE_R	  = -2.3,
		    	PLANE_I   = -1.4,
		 	PLANE_R_W =  4.0,
		    	PLANE_I_H =  4.0;
			SCREEN_PLANE_RATIO_X = 1/(double)SCREEN_W*PLANE_R_W,
			SCREEN_PLANE_RATIO_Y = 1/(double)SCREEN_H*PLANE_I_H;
			changed = true;
			break;
		case 'a':
			animatePalette = !animatePalette;
			break;
		case 'c':
			COLOR_METH = NUM_ITERS;
			changed = true;
			break;
		case 'v':
			COLOR_METH = TRAP_RADIUS;
			changed = true;
			break;
		case 'b':
			COLOR_METH = TRAP_ANGLE;
			changed = true;
			break;
		case 'i':
			SAMPLE_BLOCK_W++;
			changed = true;
			break;
		case 'k':
			SAMPLE_BLOCK_W--;
			if(SAMPLE_BLOCK_W < 1)
				SAMPLE_BLOCK_W = 1;
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
		/* XXX Broken and I don't know why
		case 's':
			unsigned char tga_pixels[SCREEN_W][SCREEN_H+256][3];
			for(int x=0; x<SCREEN_H; ++x)
				for(int y=0; y<SCREEN_W; ++y)
					for(int i=0; i<3; ++i)
						tga_pixels[x][y][i] = pixels[y][x][i];
			tga_write_raw("image.tga", 1024, 768, (unsigned char*)tga_pixels, TGA_TRUECOLOR_24);
			break;
		*/
	} // end switch(key)
} // end function keyboardDown()

void glutMouseDown(int button, int isUp, int x, int y) {
	y = SCREEN_H - y;
	switch(button) {
		case GLUT_LEFT_BUTTON:
			if(!isUp) {
				leftMouseIsDown = true;
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
				leftMouseIsDown = false;
			}
			break;
		case GLUT_RIGHT_BUTTON:
			if(!isUp) {
				zooming = ZOOM_IN;
			} else {
				zooming = ZOOM_NONE;
			}
			break;
		case GLUT_MIDDLE_BUTTON:
			if(!isUp) {
				zooming = ZOOM_OUT;
			} else {
				zooming = ZOOM_NONE;
			}
			break;
	} // end switch
} // end glutMouseDown()

void glutMouseMove(int x, int y) {
	curMouseX = x;
	curMouseY = SCREEN_H - y;
} // end glutMouseMove()

int lastTime;

void idle(void) {
	int cur_time = time(NULL);
	int delta = cur_time - lastTime;
	lastTime = cur_time;

	if(animatePalette) {
		PAL_OFFSET += PAL_CYC_RATE * delta;
		changed = true;
	}
	switch(zooming) {
		case ZOOM_NONE:
			break;
		case ZOOM_IN:
			{
			double xZoomFac = ZOOM_RATE*PLANE_R_W * delta,
			       yZoomFac = ZOOM_RATE*PLANE_I_H * delta;
			PLANE_R   += xZoomFac/2;  
			PLANE_I   += yZoomFac/2;
			PLANE_R_W -= xZoomFac; 
			PLANE_I_H -= yZoomFac;
			changed = true;
			}
			break;
		case ZOOM_OUT:
			{
			double xZoomFac = ZOOM_RATE*PLANE_R_W * delta,
			       yZoomFac = ZOOM_RATE*PLANE_I_H * delta;
			PLANE_R   -= xZoomFac/2;  
			PLANE_I   -= yZoomFac/2;
			PLANE_R_W += xZoomFac;  
			PLANE_I_H += yZoomFac;
			changed = true;
			}
			break;
	}
	switch(movingHoriz) {
		case MOV_L:
			PLANE_R -= MOV_RATE*PLANE_R_W * delta;
			changed = true;
			break;
		case MOV_R:
			PLANE_R += MOV_RATE*PLANE_R_W * delta;
			changed = true;
			break;
	}
	switch(movingVert) {
		case MOV_U:
			PLANE_I += MOV_RATE*PLANE_I_H * delta;
			changed = true;
			break;
		case MOV_D:
			PLANE_I -= MOV_RATE*PLANE_I_H * delta;
			changed = true;
			break;
	}
	if(changed) {
		slackers.clear();
		buildFractal(0, SCREEN_W, SCREEN_H, PLANE_R, PLANE_I, PLANE_R_W, PLANE_I_H);
		glTexImage2D (GL_TEXTURE_2D,
			      0,
			      GL_RGB,
			      SCREEN_W,
			      SCREEN_H+256,
			      0,
			      GL_RGB,
			      GL_UNSIGNED_BYTE,
			      pixels);
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
	if(leftMouseIsDown) {
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

int main(int argc, char ** argv) {
	if(argc > 1)
		if(argv[1][0] == '-')
			if(argv[1][1] == 'v') {
				VERBOSE = true;
				printf("Verbose mode on\n");
			}
	// Get glut started
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//	glutGameModeString("1024x768:32@60");
//	glutEnterGameMode();
	glutCreateWindow("Editefy");
	glutReshapeWindow(SCREEN_W, SCREEN_H);

	// Set up glut
	glutIgnoreKeyRepeat(0);
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardDown);
	glutMouseFunc(glutMouseDown);
	glutMotionFunc(glutMouseMove);
	glutSpecialFunc(specialKeyDown);
	glutSpecialUpFunc(specialKeyUp);

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

	sockServ = new SocketServer();
	if(!sockServ->StartListening(4444)) {
		printf("Failed to start server\n");
		return -1;
	}

	sockServ->RegNewClientCallback(newWorker);
	sockServ->RegClientLostCallback(lostWorker);

	lastTime = time(NULL);
	glutMainLoop();
	return 0;
} // end function main()
