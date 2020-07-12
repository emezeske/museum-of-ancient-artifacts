#include "spacecraft.h"

SpaceCraft::SpaceCraft(void) {
	x = y = 0.0; 
	xVel = yVel = 0.0;
	angle = 0.0;
	angularVel = 0.0;
	thrusting = rotatingCCW = rotatingCW = false;

	thrusterFlame = new Flame(32, 64, 15, textures[THRUSTER_TEX]);
	rotCWFlame   = new Flame( 8, 16, 15, textures[ROTCW_TEX]);
	rotCCWFlame  = new Flame( 8, 16, 15, textures[ROTCCW_TEX]);

	weaponL = new RocketLauncher(WEP_L);
	weaponR = new RocketLauncher(WEP_R);
} // end constructor SpaceCraft()

void SpaceCraft::BeginThrust() {
	if(turboThrusting)
		return;
	thrusting = true;
	thrusterFlame->BeginFiring();
} // end BeginThrust()

void SpaceCraft::EndThrust() {
	if(!thrusting)
		return;
	thrusting = false;
	thrusterFlame->EndFiring();
} // end EndThrust()

void SpaceCraft::BeginTurboThrust() {
	if(thrusting)
		return;
	weaponL->Retract();
	weaponR->Retract();
	thrusterFlame->ChangeColor(0x39C4EC00, 0xFF00DE00);
	thrusterFlame->BeginFiring();
	turboThrusting = true;
} // end BeginTurboThrust()

void SpaceCraft::EndTurboThrust() {
	if(!turboThrusting)
		return;
	weaponL->Extend();
	weaponR->Extend();
	thrusterFlame->ChangeColor(0x39C4EC00, 0x0712A300);
	thrusterFlame->EndFiring();
	turboThrusting = false;
} // end EndTurboThrust()

void SpaceCraft::BeginRotatingCW() {
	if(!rotatingCCW) {
		rotatingCW = true;
		rotCWFlame->BeginFiring();
	}
} // end BeginRotatingCW()
void SpaceCraft::EndRotatingCW() {
	rotatingCW = false;
	rotCWFlame->EndFiring();
} // end EndRotatingCW()

void SpaceCraft::BeginRotatingCCW() {
	if(!rotatingCW) {
		rotatingCCW = true;
		rotCCWFlame->BeginFiring();
	}
} // end BeginRotatingCCW()
void SpaceCraft::EndRotatingCCW() {
	rotatingCCW = false;
	rotCCWFlame->EndFiring();
} // end EndRotatingCCW()

void SpaceCraft::Update(int delta) {
	if(rotatingCW)
		angularVel += ROT_PWR * delta;
	else if(rotatingCCW)
		angularVel -= ROT_PWR * delta;

	angle += angularVel * delta;

	if(thrusting) {
		xVel += cosf(angle*M_PI/180) * THRUST_PWR * delta;
		yVel += sinf(angle*M_PI/180) * THRUST_PWR * delta;
	} else if(turboThrusting) {
		xVel += cosf(angle*M_PI/180) * T_THRUST_PWR * delta;
		yVel += sinf(angle*M_PI/180) * T_THRUST_PWR * delta;
	}

	x += xVel * delta;
	y += yVel * delta;

	weaponL->Update(delta);
	weaponR->Update(delta);

	thrusterFlame->Update(delta);
	rotCWFlame->Update(delta);
	rotCCWFlame->Update(delta);
} // end Update()

void SpaceCraft::Draw(void) {
	glPushMatrix();
	glTranslatef(SCREEN_WIDTH*0.5, SCREEN_HEIGHT*0.5, 0.0);
	glRotatef(angle, 0.0, 0.0, 1.0);

	glBindTexture(GL_TEXTURE_2D, textures[SHIP_TEX]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(-50, -25);
		glTexCoord2f(0, 1);
		glVertex2f(-50,  25);
		glTexCoord2f(1, 1);
		glVertex2f( 50,  25);
		glTexCoord2f(1, 0);
		glVertex2f( 50, -25);
	glEnd();

	int flameLen;
	flameLen = (turboThrusting) ? 150 : 80;
	glBindTexture(GL_TEXTURE_2D, textures[THRUSTER_TEX]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 1);
		glVertex2f(-50-flameLen, -7);
		glTexCoord2f(1, 1);
		glVertex2f(-50-flameLen,  7);
		glTexCoord2f(1, 0);
		glVertex2f( -50,  7);
		glTexCoord2f(0, 0);
		glVertex2f( -50, -7);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textures[ROTCW_TEX]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(-36,  19);
		glTexCoord2f(0, 1);
		glVertex2f(-36,  52);
		glTexCoord2f(1, 1);
		glVertex2f(-44,  52);
		glTexCoord2f(1, 0);
		glVertex2f(-44,  19);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textures[ROTCCW_TEX]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(-36,  -19);
		glTexCoord2f(0, 1);
		glVertex2f(-36,  -52);
		glTexCoord2f(1, 1);
		glVertex2f(-44,  -52);
		glTexCoord2f(1, 0);
		glVertex2f(-44,  -19);
	glEnd();

	weaponL->Draw();
	weaponR->Draw();

	glPopMatrix();
} // end Draw()
