 /*******************************************************
 * - 2DGE (2D Game engine) -- Written by Evan Mezeske -	*
 *							*
 * The innards of an up and coming 2D game that 	*
 * so far can draw any number of dynamic lights		*
 * that can move, change color, and change in		*
 * intensity.  The engine also applies these lights	*
 * to a bumpmapped surface, giving the 2D scene		*
 * a very realistic and quasi-3D appearance.		*
 * Written using the GLUT and GLEW and OpenGL		*
 * libraries for nice cross-platform abilities and	*
 * using a small library for loading *.tga files	*
 * because Evan didn't feel like writing his own.	*
 * This *should* compile under windows / mac but	*
 * this has NOT been tested yet.  This main.cc		*
 * is pretty much just a driver for testing the other	*
 * components right now...				*
 *							*
 *******************************************************/

#include "glmanager.h"
#include "vec3d.h"
#include "background.h"
#include "light.h"

// This object takes care of setting up OpenGL and the rendering engine
GLManager *glman = NULL;
// A temporary background object to demonstrate the IsDrawable interface
// and to show some graphics to make sure the renderer works
BackGround *bg = NULL;
// A pointer to some lights just for testing purposes
Light* testLights = NULL;

// This function just sets up the necessary objects for rendering the scene
// with some lights and a background to look at for testing
void initGame(int argc, char **argv) {
	glman 		= new GLManager(&argc, argv);
	bg 		= new BackGround();
	// Use operator new to allocate raw memory, and cast it into a Light*
	testLights = (Light*)(operator new(sizeof(Light)*3));
	// Use placement new to call constructors for light objects in the freshly
	// allocated space.  I know it looks silly, but it's the only way to call
	// ctors that take arguments for an array of Lights.  When the Light ctor
	// is called, the Light gets added to the renderer's list of Lights to draw.
	// I didn't want this list to contain uninitialized Lights, so I did this.
	new ((void*)&testLights[0]) Light(Vec3D(256, 256, 30), 1.0, 0.0, 0.0, 0.5);
	new ((void*)&testLights[1]) Light(Vec3D(400, 400, 20), 1.0, 1.0, 1.0, 0.4);
	new ((void*)&testLights[2]) Light(Vec3D(100, 100, 30), 0.0, 0.0, 1.0, 0.5);
} // end fnc initGame()

// Prevent memory leaks
void cleanup_and_exit(void) {
	// The testLights were allocated with operator new, so they should
	// be deleted with operator delete.
	operator delete(testLights);
	// Regular cleanup
	delete bg;
	delete glman;
	// Seeya later!
	exit(0);
} // end fnc cleanup_and_exit()

// Start the ball rolling
int main(int argc, char **argv) {
	initGame(argc, argv);
	glutMainLoop();
	return 0;
} // end fnc main()
