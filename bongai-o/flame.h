#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include "defines.h"

class Flame {
	// Pixels for thruster
	unsigned int * pixels;
	// Time accumulator for update rate
	int timeAccum;
	// Update rate
	int updateRate;
	// Width and height of flame
	int W, H;
	// Is it currently firing?
	bool firing;
	// Texture to bind to
	GLuint texture;
	int innerColor, outerColor;

	public:
	// Pass constructor width, height, update rate, and texture
	Flame(int, int, int, GLuint);
	void BeginFiring(void);
	void EndFiring(void);
	void ChangeColor(int, int);
	void Update(int);
};
