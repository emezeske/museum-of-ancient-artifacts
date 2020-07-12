#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include "defines.h"
#include "flame.h"
#include "weapons.h"

#define THRUST_PWR 0.0003
#define T_THRUST_PWR 0.0013
#define ROT_PWR    0.0003

extern GLuint * textures;

class SpaceCraft {
	public:
	float 	x, y,
		xVel, yVel,
		angle,
		angularVel;
	bool 	thrusting,
		turboThrusting,
		rotatingCW,
		rotatingCCW;
	// Thruster images
	Flame *	thrusterFlame, * rotCWFlame, * rotCCWFlame;
	// Weapons!!
	Weapon * weaponL, * weaponR;

	SpaceCraft(void);
	void BeginThrust(void);
	void EndThrust(void);
	void BeginTurboThrust(void);
	void EndTurboThrust(void);
	void BeginRotatingCW(void);
	void EndRotatingCW(void);
	void BeginRotatingCCW(void);
	void EndRotatingCCW(void);
	void Update(int);
	void Draw(void);
}; // end class SpaceCraft
