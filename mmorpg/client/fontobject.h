///////////////////////////////////////////////////////////////////////////////
// Filename: fontobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _FONTOBJECT_H_
#define _FONTOBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <fstream>
#include <string>
using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "openglobject.h"
#include "textureobject.h"


///////////////////////////////////////////////////////////////////////////////
// Class name: FontObject
///////////////////////////////////////////////////////////////////////////////
class FontObject
{
private:
	struct FontType
	{
		float left, right;
		int size;
	};

public:
	FontObject();
	FontObject(const FontObject&);
	~FontObject();

	bool Initialize(char*, char*);
	bool Shutdown();

	void DrawSentence(OpenGLObject*, string, int, int, float, float, float);

private:
	TextureObject* FontTexture;  // The texture for the font.
	FontType* Font;              // The font structure.
};

#endif
