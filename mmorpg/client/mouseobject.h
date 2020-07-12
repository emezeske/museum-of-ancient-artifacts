///////////////////////////////////////////////////////////////////////////////
// Filename: mouseobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _MOUSEOBJECT_H_
#define _MOUSEOBJECT_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureobject.h"
#include "openglobject.h"


///////////////////////////////////////////////////////////////////////////////
// Class name: MouseObject
///////////////////////////////////////////////////////////////////////////////
class MouseObject
{
public:
	MouseObject();                    // Constructor.
	MouseObject(const MouseObject&);  // Copy constructor.
	~MouseObject();                   // Destructor.

	bool Initialize();           // Initializes the mouse cursor.
	bool Shutdown();             // Shuts down the mouse cursor.
	bool Render(OpenGLObject*);  // Renders the mouse cursor.

	void SetPosition(int, int);  // Sets the position of the cursor.
	void SetButton(bool);        // Sets the left button down.
	int getX();
	int getY();

private:
	TextureObject* MouseTexture;  // The textures for the mouse.
	int xPos, yPos;               // The position of the mouse.
	bool mouseDown;               // If the left mouse button is down.
};

#endif
