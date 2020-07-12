#include "textures.h"
#include "worldobject.h"
#include "fixedworldobject.h"

extern GLuint * textures;
extern int numTextures;
extern FixedWorldObject * fixedWorldObject;

void loadTextures(void) {

	// open the texture list file and store names of files in the texture_filenames array
	FILE * texture_list_file = fopen("./gfx/texturelist", "r");	
	
	while(!feof(texture_list_file)) {
		if(fgetc(texture_list_file) == '\n') 
			++numTextures;
	} // end while

	rewind(texture_list_file);

	// dynamically make an array of filenames
	char **texture_filenames = new char*[numTextures];
	for(int i = 0; i < numTextures; ++i) 
		texture_filenames[i] = new char[32];

	char temp;

	for(int i = 0; i < numTextures ; ++i) {

		int z = 0;

		while(!feof(texture_list_file)) {
			temp = fgetc(texture_list_file);
			if(temp != '\n') texture_filenames[i][z] = temp;
			else break;
			++z;
		} // end while

		texture_filenames[i][z] = '\0';

	} // end for i	

	fclose(texture_list_file);

	// load all textures specified in texture_filenames array
	LTGA *l;
	textures = new GLuint[numTextures];
	glGenTextures(numTextures, textures);
	fixedWorldObject = new FixedWorldObject[numTextures-1];

	for(int i = 0; i<numTextures; ++i) {
		
		l = new LTGA(texture_filenames[i]);

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
	
		if(i != 0) {
			fixedWorldObject[i-1].width         = l->GetImageWidth();
			fixedWorldObject[i-1].height        = l->GetImageHeight();
			fixedWorldObject[i-1].textureNumber = i;
		} // end if

	delete l;	

	} // end for i

	for(int i = 0; i < numTextures; ++i)
		delete [] texture_filenames[i];
	delete [] texture_filenames;

} // end loadGraphics()
