///////////////////////////////////////////////////////////////////////////////
// Filename: fontobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "fontobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: FontObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Initializes the pointers to null.
///////////////////////////////////////////////////////////////////////////////
FontObject::FontObject()
{
	FontTexture = NULL;
	Font = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: FontObject
// Purpose: Copy constructor.
// Inputs: other - The font object this function is making a copy of.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
FontObject::FontObject(const FontObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~FontObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: Releases the memory if it has not been done so already.
///////////////////////////////////////////////////////////////////////////////
FontObject::~FontObject()
{
	if(FontTexture)
	{
		delete FontTexture;
		FontTexture = NULL;
	}
	if(Font)
	{
		delete [] Font;
		Font = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Initialize
// Purpose: Initializes the font object so that 2D text can be drawn onscreen.
// Inputs: filename - Filename of the texture file for the font.
//         dataFilename - Filename of the file with the size and spacing data
//                        for the font.
// Outputs: A boolean value inidicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool FontObject::Initialize(char* filename, char* dataFilename)
{
	bool result;
	ifstream fin;
	int i;
	char temp;


	// Create the font texture object.
	FontTexture = new TextureObject;
	
	// Load the texture data into the object using transparency.
	result = FontTexture->Load(filename);
	if(!result)
	{
		return false;
	}

	// Create the font spacing buffer.
	Font = new FontType[95];

	// Read in the font size and spacing between chars.
	fin.open(dataFilename);

	// Read in the 95 used ascii characters for text.
	for(i=0; i<95; i++)
	{
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}

		fin >> Font[i].left;
		fin >> Font[i].right;
		fin >> Font[i].size;
	}

	// Close the file.
	fin.close();

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Shutdown
// Purpose: Shuts down the font object and releases the memory.
// Inputs: None. 
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool FontObject::Shutdown()
{
	// Release the font texture.
	if(FontTexture)
	{
		FontTexture->Free();
		delete FontTexture;
		FontTexture = NULL;
	}

	// Release the font data.
	if(Font)
	{
		delete [] Font;
		Font = NULL;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: DrawSentence
// Purpose: Draws a text sentence to the screen.
// Inputs: Graphics - The OpenGL graphics object pointer.
//         sentence - The sentence to be drawn.
//         numLetters - How many letters are in the sentence.
//         posX - The x position to draw the sentence at.
//         posY - The y position to draw the sentence at.
//         red - The red component of the text color desired.
//         green - The green component of the text color desired.
//         blue - The blue component of the text color desired.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void FontObject::DrawSentence(OpenGLObject* Graphics, string sentence, int posX, int posY,
			      float red, float green, float blue)
{
	int i, letter;
	int numLetters = sentence.length();

	// Save the state on the stack.
	glPushAttrib(GL_TEXTURE_BIT | GL_LIGHTING_BIT);

	// Turn on texturing and turn off lighting.
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	// Turn on blending.
	glAlphaFunc(GL_GREATER, 0.2f);
	glEnable(GL_ALPHA_TEST);

	// Go into orthographic drawing mode.
	Graphics->OrthoMode();

	// Set the texture for the font.
	FontTexture->Bind();

	// Set the color of the text to be drawn.
	glColor4f(red, green, blue, 1.0f);

	// Reset the drawing position to the position the text should be drawn at.
	glLoadIdentity();
	glTranslated(posX, posY, 0);

	// Draw each letter onto a quad.
	for(i=0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if(letter == 0)
		{
			glTranslated(3, 0, 0);
		}
		else
		{
			glBegin(GL_QUADS);
				glTexCoord2f(Font[letter].left, 0.0f); 
				glVertex2i(0, 16);

				glTexCoord2f(Font[letter].right, 0.0f); 
				glVertex2i(Font[letter].size, 16);
				
				glTexCoord2f(Font[letter].right, 1.0f); 
				glVertex2i(Font[letter].size, 0);
				
				glTexCoord2f(Font[letter].left, 1.0f); 
				glVertex2i(0, 0);
			glEnd();
			glTranslated((Font[letter].size + 1), 0, 0);
		}
	}

	// Reset the color.
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	// Go back into perspective mode for drawing.
	Graphics->PerspectiveMode();

	// Turn off blending.
	glDisable(GL_ALPHA_TEST);

	// Pop the state off the stack so it is the same as before entering this 
	// function.
	glPopAttrib();

	return;
}
