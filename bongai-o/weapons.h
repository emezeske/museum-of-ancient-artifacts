#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include "defines.h"

enum WEAPON_SIDE  { WEP_L, WEP_R };
enum WEAPON_STATE { WEP_EXTENDED, WEP_RETRACTED, WEP_EXTENDING, WEP_RETRACTING };

// Abstract weapon interface -- all weapons will be derived from this
class Weapon {
	public:
	GLuint  texture;
	WEAPON_SIDE side;
	WEAPON_STATE state;
	float x, y, w, h;

	virtual void Retract(void) 	= 0;
	virtual void Extend(void) 	= 0;
	virtual void Draw(void) 	= 0;
	virtual void Update(int) 	= 0;
}; // end class Weapon

class RocketLauncher : public Weapon { 
	float redLightOpac;

	public:
	RocketLauncher(WEAPON_SIDE);

	void Retract(void);
	void Extend(void);
	void Draw(void);
	void Update(int);
}; // end class RocketLauncher
