#include "weapons.h"

extern GLuint * textures;

#define RETRACTED_POS 2
#define EXTENDED_POS  10
#define EXTENSION_RATE 0.03

RocketLauncher::RocketLauncher(WEAPON_SIDE wepSide) {
	side = wepSide;
	texture = textures[ROCK_LAUNCH_TEX];
	state = WEP_EXTENDED;
	x = -23;
	y = EXTENDED_POS;
	w = 40;
	h = 17;
	redLightOpac = 1.0;
} // end constructor RocketLauncher()

void RocketLauncher::Retract() {
	state = WEP_RETRACTING;
} // end Retract()

void RocketLauncher::Extend() {
	state = WEP_EXTENDING;
} // end Extend()

void RocketLauncher::Update(int delta) {
	switch(state) {
		case WEP_EXTENDING:
			y += EXTENSION_RATE * delta;
			if(y > EXTENDED_POS) {
				y = EXTENDED_POS;
				state = WEP_EXTENDED;
			}
			break;
		case WEP_RETRACTING:
			y -= EXTENSION_RATE * delta;
			if(y < RETRACTED_POS) {
				y = RETRACTED_POS;
				state = WEP_RETRACTED;
			}
	} // end switch

	redLightOpac -= 0.001 * delta;
	if(redLightOpac < 0)
		redLightOpac = 1.0;
} // end Update()

void RocketLauncher::Draw() {
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	switch(side) {
		case WEP_L:
			glTexCoord2f(0, 0);
			glVertex2f(x, y);
			glTexCoord2f(0, 1);
			glVertex2f(x, y+h);
			glTexCoord2f(1, 1);
			glVertex2f(x+w, y+h);
			glTexCoord2f(1, 0);
			glVertex2f(x+w, y);
			break;
		case WEP_R:
			glTexCoord2f(0, 0);
			glVertex2f(x, -y);
			glTexCoord2f(0, 1);
			glVertex2f(x, -y-h);
			glTexCoord2f(1, 1);
			glVertex2f(x+w, -y-h);
			glTexCoord2f(1, 0);
			glVertex2f(x+w, -y);
	} // end switch
	glEnd();

	glColor4f(1.0, 0.0, 0.0, redLightOpac);
	glDisable(GL_TEXTURE_2D);
	glPointSize(3);
	glBegin(GL_POINTS);
	switch(side) {
		case WEP_L:
			glVertex2f(x+w-4, y+0.5*h-2);
			break;
		case WEP_R:
			glVertex2f(x+w-4, -y-0.5*h+2);
			break;
	} // end switch
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0, 1.0, 1.0, 1.0);
} // end Draw()
