#ifndef __GLMANAGER_H__
#define __GLMANAGER_H__

#include <GL/glew.h>
#include <GL/glut.h>
#include "renderer.h"

class GLManager {
	private:
		// This stuff has to be static because the callback functions have
		// to be static (for callback registration) and need access to these
		// variables
		static int fps;			// for keeping track of frames per second
		static bool rotateLight;	// does the user want the light to rotate?
		static Renderer *renderer;	// the object that actually draws geometry
		// Print out the frames per second occationally
		static void updateFPSCounter(int);
		// GLUT CALLBACK FNCS
		// GLUT keyboard down callback fnc
		static void keyDown(unsigned char, int, int);
		// GLUT mouse down callback fnc
		static void mouseDown(int, int, int, int);
		// GLUT mouse motion callback fnc
		static void mouseMove(int, int);
		// GLUT callback fnc to draw to the back buffer and swap buffers
		static void display(void);
		// GLUT callback fnc for between-frame calculation
		static void idle(void);
	public:
		// Constructor takes command line arguments to pass to glutInit()
		// and then sets up the GLUT, GLEW, and GL environment
		GLManager(int*, char**);
		// Cleanup
		~GLManager();
}; // end class GLManager
#endif
