#include "textures.h"
#include <iostream>

extern GLuint * textures;

void loadTextures(void) {

	LTGA * l;
	textures = new GLuint;
	glGenTextures(MAX_TEXTURES, textures);

	char texFileNames[MAX_TEXTURES][32] = { "./graphics/skier.tga", 
						"./graphics/tree.tga" };

	for(int i = 0; i<MAX_TEXTURES; ++i) {

		l = new LTGA(texFileNames[i]);
		
		glBindTexture(GL_TEXTURE_2D, textures[i]);
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

	} // end for i
	
	delete l;	

} // end loadTextures()
