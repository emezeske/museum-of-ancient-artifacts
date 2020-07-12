///////////////////////////////////////////////////////////////////////////////
// Filename: textureobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "textureobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: TextureObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Initializes the texture pointers.
///////////////////////////////////////////////////////////////////////////////
TextureObject::TextureObject()
{
	textureLoaded = false;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: TextureObject
// Purpose: Copy constructor.
// Inputs: other - The object this function is making a copy of.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
TextureObject::TextureObject(const TextureObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~TextureObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: Releases texture memory.
///////////////////////////////////////////////////////////////////////////////
TextureObject::~TextureObject()
{
	Free();
}

///////////////////////////////////////////////////////////////////////////////
// Function name: Load
// Purpose: Loads the texture file in.
// Inputs: filename - The file name of the texture.
// Outputs: A boolean value indicating success.
// Details: This function works for targa format.
///////////////////////////////////////////////////////////////////////////////
bool TextureObject::Load(char* filename)
{
	Free();
	textureLoaded = false;

	// Load the data from the file.
	std::string fileNameString = filename;
	LTGA * ltgaObject = new LTGA(filename);

	// If it was loaded then setup the texture.
	if(ltgaObject != 0)
	{
		// Set the texture to being in the loaded state.
		textureLoaded = true;

		// Generate the texture object name.
		glGenTextures(1, &textureOpenGL);

		// Bind the texture for use.
		glBindTexture(GL_TEXTURE_2D, textureOpenGL);

		// Load the texture and use mipmapping.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		if(!ltgaObject->GetAlphaDepth() )
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, ltgaObject->GetImageWidth(), ltgaObject->GetImageHeight(), 
					  GL_RGB, GL_UNSIGNED_BYTE, ltgaObject->GetPixels() );
		else
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, ltgaObject->GetImageWidth(), ltgaObject->GetImageHeight(), 
					  GL_RGBA, GL_UNSIGNED_BYTE, ltgaObject->GetPixels() );


		delete ltgaObject;
		ltgaObject = 0;
	}

	return textureLoaded;
}

///////////////////////////////////////////////////////////////////////////////
// Function name: Free
// Purpose: Releases the texture data.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void TextureObject::Free()
{
	if(glIsTexture(textureOpenGL))
	{
		glDeleteTextures(1, &textureOpenGL);
	}

	textureLoaded = false;

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Bind
// Purpose: Sets this texture as the color map for current drawing on polygons.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void TextureObject::Bind()
{
	if(textureLoaded)
	{
		glBindTexture(GL_TEXTURE_2D, textureOpenGL);
	}
	return;
}
