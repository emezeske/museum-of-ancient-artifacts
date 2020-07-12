#include <GL/glut.h>
#include <string>
#include "ltga.h"
#include "textdisplayer.h"

using namespace std;

GLuint * fontTextures;

float CHAR_SIZE         = 10; 
float CHAR_SPACING      = CHAR_SIZE * 0.75; 
float CHAR_LINE_SPACING = CHAR_SIZE * 0.60; 
float WEIRD_CHAR_OFFSET = CHAR_SIZE * 0.25; 


void fontSet(fontSetOption setWhat, float rhs) {

	if(setWhat == FONT_SIZE) {
		CHAR_SIZE = rhs;
		CHAR_SPACING      = CHAR_SIZE * 0.75; 
		CHAR_LINE_SPACING = CHAR_SIZE * 0.60; 
		WEIRD_CHAR_OFFSET = CHAR_SIZE * 0.25; 
	}

} // end fontSet()

string numberToString(int tempInt) {

	string tempString;
	string numbers = "0123456789";

	if(tempInt == 0) { tempString += numbers[0]; return tempString; }

	if(tempInt >= 100000)
		tempString += numbers[int(tempInt / 100000 % 10)];
	if(tempInt >= 10000)
		tempString += numbers[int(tempInt / 10000  % 10)];
	if(tempInt >= 1000)
		tempString += numbers[int(tempInt / 1000   % 10)];
	if(tempInt >= 100)
		tempString += numbers[int(tempInt / 100    % 10)];
	if(tempInt >= 10)
		tempString += numbers[int(tempInt / 10     % 10)];
	if(tempInt >= 1)
		tempString += numbers[int(tempInt          % 10)];

	while(tempString[0] == '0')
			tempString.erase(0, 1);

	return tempString;

} // end numberToString(int)

void drawTextBox(string textString, float x, float y, float width) {

	float curX = x;
	float curY = y;
	width += x;

	float maxX = 0;

	// figure out how big the box should be and draw it
	for(int i = 0; i < textString.length(); ++i) {
		int tempCount = i;
		float tempX   = curX;
		if(tempX > maxX)
			maxX = tempX;
		while(textString[tempCount] != ' ' && textString[tempCount] != '\0') {

			if(tempX + CHAR_SPACING >= width) {
				curX = x;
				curY -= CHAR_SPACING + CHAR_LINE_SPACING;
				break;
			} // end if

			++tempCount;
			tempX += CHAR_SPACING;
		
		} // end while

		curX += CHAR_SPACING;
	} // end for

	drawBorderBox(x, y, maxX, curY);

	curX = x;
	curY = y;

	// draw all the characters in the string within the width
	for(int i = 0; i < textString.length(); ++i) {
		
		int tempCount = i;
		float tempX   = curX;
		while(textString[tempCount] != ' ' && textString[tempCount] != '\0') {

			if(tempX + CHAR_SPACING >= width) {
				curX = x;
				curY -= CHAR_SPACING + CHAR_LINE_SPACING;
				break;
			} // end if

			++tempCount;
			tempX += CHAR_SPACING;
		
		} // end while

		if(textString[i] != ' ') {
				drawAChar(curX, curY, char(textString[i]) );
		} // end if

		curX += CHAR_SPACING;

	} // end for

} // end drawTextBox

void drawBorderBox(float x, float y, float width, float height) {

	glPushMatrix();

	glBegin(GL_QUADS);
	glColor4f(0.0, 0.0, 1.0, 0.1);
	glVertex2f(		   x, height - WEIRD_CHAR_OFFSET);
	glColor4f(0.0, 0.0, 1.0, 0.8);
	glVertex2f(		   x, y + CHAR_SIZE);
	glColor4f(0.0, 0.0, 1.0, 0.1);
	glVertex2f(width + CHAR_SIZE, y + CHAR_SIZE);
	glColor4f(0.0, 0.0, 1.0, 0.8);
	glVertex2f(width + CHAR_SIZE, height - WEIRD_CHAR_OFFSET);
	glEnd();

	glPopMatrix();

} // end drawBorderBox()

void drawAChar(float x, float y, char tempChar) {

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	// WEIRD_CHAR_OFFSET is to compensate for characters
	// that have tails that dip down... damn them
	if(     tempChar == 'g' ||
		tempChar == 'j' ||
		tempChar == 'p' ||
		tempChar == 'q' ||
		tempChar == 'y'   ) 
		glTranslatef(x, y - WEIRD_CHAR_OFFSET, 0.0);
	else
		glTranslatef(x, y, 0.0);

	int charToDraw;
	if(int(tempChar) <= 90 && int(tempChar) >= 65)
		charToDraw = int(tempChar) - 65;
	if(int(tempChar) <= 122 && int(tempChar) >= 97)
		charToDraw = int(tempChar) - 71;
	if(int(tempChar) <= 57 && int(tempChar) >= 48)
		charToDraw = int(tempChar) + 4;
	if(tempChar == ':')
		charToDraw = 62;

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, fontTextures[charToDraw]);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0      , 0.0);
	glVertex2f(  0.0      , 0.0);
	glTexCoord2f(0.0      , 1.0);
	glVertex2f(  0.0      , CHAR_SIZE);
	glTexCoord2f(1.0      , 1.0);
	glVertex2f(  CHAR_SIZE, CHAR_SIZE);
	glTexCoord2f(1.0      , 0.0);
	glVertex2f(  CHAR_SIZE, 0.0);
	glEnd();

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

} // end drawAChar()

void initTextDisplayer(void) {

	const int numChars = 63;

	LTGA *l;
	fontTextures = new GLuint[numChars];
	char font_filenames[numChars][32] = {   "./gfx/fontquads/A.tga", 
						"./gfx/fontquads/B.tga",
						"./gfx/fontquads/C.tga",
						"./gfx/fontquads/D.tga",
						"./gfx/fontquads/E.tga",
						"./gfx/fontquads/F.tga",
						"./gfx/fontquads/G.tga",
						"./gfx/fontquads/H.tga",
						"./gfx/fontquads/I.tga",
						"./gfx/fontquads/J.tga",
						"./gfx/fontquads/K.tga",
						"./gfx/fontquads/L.tga",
						"./gfx/fontquads/M.tga",
						"./gfx/fontquads/N.tga",
						"./gfx/fontquads/O.tga",
						"./gfx/fontquads/P.tga",
						"./gfx/fontquads/Q.tga",
						"./gfx/fontquads/R.tga",
						"./gfx/fontquads/S.tga",
						"./gfx/fontquads/T.tga",
						"./gfx/fontquads/U.tga",
						"./gfx/fontquads/V.tga",
						"./gfx/fontquads/W.tga",
						"./gfx/fontquads/X.tga",
						"./gfx/fontquads/Y.tga",
						"./gfx/fontquads/Z.tga",
						"./gfx/fontquads/a.tga",
				 		"./gfx/fontquads/b.tga",
				 		"./gfx/fontquads/c.tga",
				 		"./gfx/fontquads/d.tga",
				 		"./gfx/fontquads/e.tga",
				 		"./gfx/fontquads/f.tga",
				 		"./gfx/fontquads/g.tga",
				 		"./gfx/fontquads/h.tga",
				 		"./gfx/fontquads/i.tga",
				 		"./gfx/fontquads/j.tga",
				 		"./gfx/fontquads/k.tga",
				 		"./gfx/fontquads/l.tga",
				 		"./gfx/fontquads/m.tga",
				 		"./gfx/fontquads/n.tga",
				 		"./gfx/fontquads/o.tga",
				 		"./gfx/fontquads/p.tga",
				 		"./gfx/fontquads/q.tga",
				 		"./gfx/fontquads/r.tga",
				 		"./gfx/fontquads/s.tga",
				 		"./gfx/fontquads/t.tga",
				 		"./gfx/fontquads/u.tga",
				 		"./gfx/fontquads/v.tga",
				 		"./gfx/fontquads/w.tga",
				 		"./gfx/fontquads/x.tga",
				 		"./gfx/fontquads/y.tga",
				 		"./gfx/fontquads/z.tga",
				 		"./gfx/fontquads/0.tga",
				 		"./gfx/fontquads/1.tga",
				 		"./gfx/fontquads/2.tga",
				 		"./gfx/fontquads/3.tga",
				 		"./gfx/fontquads/4.tga",
				 		"./gfx/fontquads/5.tga",
				 		"./gfx/fontquads/6.tga",
				 		"./gfx/fontquads/7.tga",
				 		"./gfx/fontquads/8.tga",
				 		"./gfx/fontquads/9.tga",
				 		"./gfx/fontquads/colon.tga",
						};

	glGenTextures(numChars, fontTextures);

	for(int i = 0; i < numChars; ++i) {
		
		l = new LTGA(font_filenames[i]);

		glBindTexture(GL_TEXTURE_2D, fontTextures[i]);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		glTexImage2D (GL_TEXTURE_2D,
			      0,
			      GL_RGBA,
			      l->GetImageWidth(),
			      l->GetImageHeight(),
			      0,
			      GL_RGBA,
			      GL_UNSIGNED_BYTE,
			      l->GetPixels() );
	
		delete l;	

	} // end for i
	

} // end loadFontTextures()
