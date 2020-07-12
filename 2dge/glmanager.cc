#include <stdio.h>
#include "glmanager.h"

// The evil externs:
// If user exits program, main cleanup function must be called
extern void cleanup_and_exit(void);
// The width and height of the window in pixels (although at this point the pixels
// line up with the object space coordinates, but this might change)
extern int 	SCREEN_W,
       		SCREEN_H;

// Static variables
int 		GLManager::fps		= 0;
bool 		GLManager::rotateLight	= true;
Renderer 	*GLManager::renderer	= NULL;

// The GLManager ctor gets GLUT/GLEW up and running, registers callback functions,
// and creates a window
GLManager::GLManager(int* p_argc, char** argv) {
	glutInit(p_argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//	Uncomment these two lines and comment the next two for fullscreen mode
//	glutGameModeString("800x600:32@60");
//	glutEnterGameMode();
	glutCreateWindow("game");
	glutReshapeWindow(SCREEN_W, SCREEN_H);
	gluOrtho2D(0, SCREEN_W, 0, SCREEN_H);

	// Register GLUT callbacks
	glutKeyboardFunc(keyDown);
	glutMouseFunc(mouseDown);
	glutMotionFunc(mouseMove);
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	// Start up GLEW, and make sure that the OpenGL extension we need
	// is supported.
	GLenum stat = glewInit();
	if(stat != GLEW_OK) {
		printf("FATAL: glew failed to initialize. Glew error string: \"%s\"\n", glewGetErrorString(stat));
		cleanup_and_exit();
	}
	//if(!GLEW_ARB_texture_rectangle) {
	//	printf("FATAL: no support for GL_ARB_texture_rectangle extension\n");
	//	cleanup_and_exit();
	//} 
	renderer = new Renderer();
} // end ctor GLManager()

// Cleanup!
GLManager::~GLManager(void) {
	delete renderer;
} // end dtor GLManager()

// This function allows the user to press ESC to exit the program and press
// the spacebar to stop the light from rotating
void GLManager::keyDown(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			cleanup_and_exit();
			break;
		case ' ':
			rotateLight = !rotateLight;
			break;
	} // end switch(key)
} // end fnc keyDown()

// Doesn't do anything yet
void GLManager::mouseDown(int button, int isUp, int x, int y) {
} // end mouseDown

// Doesn't do anything yet
void GLManager::mouseMove(int x, int y) {
} // end mouseMove()

// This is the GLUT callback function that should draw everything.
void GLManager::display(void) {
	// Go through the whole rendering process
	renderer->DrawScene();
	++fps;
	glutSwapBuffers();
} // end fnc display()

// This is the GLUT callback function that gets called whenever there's nothing
// that GLUT needs to be doing.  It is mainly used for updating objects that are
// dynamic with respect to time.
void GLManager::idle(void) {
	// Calculate delta, which is the number of milliseconds since
	// the last call of idle().  This is used to provide framerate
	// independant object motion
	static int lastTime = glutGet(GLUT_ELAPSED_TIME);
	int time = glutGet(GLUT_ELAPSED_TIME);
	int delta = time - lastTime;
	lastTime = time;
	// Count the frames per second
	updateFPSCounter(delta);
	// Move the lights around for testing
	if(rotateLight)
		renderer->WiggleLights(delta);
	// This function gells GLUT that the scene is ready to be rendered
	// at GLUT's disposal
	glutPostRedisplay();
} // end fnc idle()

// This is just the stuff to print out the number of frames
// rendered in the previous second
void GLManager::updateFPSCounter(int delta) {
	static int fps_accum = 0;
	fps_accum += delta;
	if(fps_accum > 1000) {
		printf("FPS: %d\n", fps);
		fps = fps_accum = 0;
	}
} // end fnc updateFPSCounter()
