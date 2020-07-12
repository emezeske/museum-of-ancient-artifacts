#include "background.h"
#include "utils.h"

extern int SCREEN_W;
extern int SCREEN_H;

BackGround::BackGround() {
	// All the ctor does right now is setup and load the 
	// targa texture files for use in drawing
	glGenTextures(1, &tex);
	loadTexture(tex, "./gfx/bgtex.tga");
	glGenTextures(1, &bmtex);
	loadTexture(bmtex, "./gfx/bmtex.tga");
} // end ctor BackGround()

void BackGround::DrawBumpMap() {
	DrawFullScreenQuad(bmtex);
} // end fnc drawBumpMap()

void BackGround::Draw() {
	DrawFullScreenQuad(tex);
} // end fnc draw()

void BackGround::DrawFullScreenQuad(GLuint tex) {
	// Bind the correct texture and then draw a quad that covers the screen
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);
		glTexCoord2f(	0			, 0);
		glVertex2f(	0			, 0);
		glTexCoord2f(	0			, (float)SCREEN_H/texSize);
		glVertex2f(	0			, SCREEN_H);
		glTexCoord2f(	(float)SCREEN_W/texSize	, (float)SCREEN_H/texSize);
		glVertex2f(	SCREEN_W		, SCREEN_H);
		glTexCoord2f(	(float)SCREEN_W/texSize	, 0);
		glVertex2f(	SCREEN_W		, 0);
	glEnd();
} // end fnc DrawFullScreenQuad()
