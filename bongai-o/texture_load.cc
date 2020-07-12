#include "ltga.h"

void penis( void )
{
	const int NUM_TEXTURES = 1;
	const int BG_TEX = 0;
	
	textures = new GLuint[NUM_TEXTURES];
	
	loadTGATexture("orionnebula.tga", BG_TEX, false);
	// Do this in the display function
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[BG_TEX]);
	// Draw stuff
}

void loadTGATexture(const char * filename, int index, bool clamp) {
	LTGA * l = new LTGA(filename);
	if(l == 0)
	{
		printf("Error loading texture\n");
		exit(0);
	}

	glBindTexture(GL_TEXTURE_2D, textures[index]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if(clamp) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	// Determine whether the image has transparency or not and load accordingly
	GLint format = (l->GetAlphaDepth()) ? GL_RGBA : GL_RGB;
	glTexImage2D (GL_TEXTURE_2D,
		      0,
		      format,
		      l->GetImageWidth(),
		      l->GetImageHeight(),
		      0,
		      format,
		      GL_UNSIGNED_BYTE,
		      l->GetPixels() );
	delete l;	
} // end loadTGATexture()
