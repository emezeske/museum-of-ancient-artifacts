///////////////////////////////////////////////////////////////////////////////
// Filename: mouseobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "mouseobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: MouseObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
MouseObject::MouseObject()
{
	MouseTexture = 0;  // Set the textures to null.
	xPos = yPos = 0;      // Initialize the position to zero.
	mouseDown = false;    // Set the left mouse button to not down.
}


///////////////////////////////////////////////////////////////////////////////
// Function name: MouseObject
// Purpose: Copy constructor.
// Inputs: other - The mouse cursor object to be copied into this object.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
MouseObject::MouseObject(const MouseObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~MouseObject
// Purpose: Destructor.
// Inputs: None,
// Outputs: None.
// Details: Releases memory in case it hasn't been done so already.
///////////////////////////////////////////////////////////////////////////////
MouseObject::~MouseObject()
{
	if(MouseTexture)
	{
		delete [] MouseTexture;
		MouseTexture = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Initialize
// Purpose: Initializes the mouse cursor.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool MouseObject::Initialize()
{
	bool result;

	
	// Create texture objects for different mouse cursors.
	MouseTexture = new TextureObject[2];

	// Load a texture for one of the mouse cursors.
	result = MouseTexture[0].Load("data/mouse1.tga");
	if(!result)
	{
		return false;
	}

	// Load a texture for the second mouse cursor.
	result = MouseTexture[1].Load("data/mouse2.tga");
	if(!result)
	{
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Shutdown
// Purpose: Releases the memory and shuts down the mouse cursor.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool MouseObject::Shutdown()
{
	if(MouseTexture)
	{
		MouseTexture[0].Free();
		MouseTexture[1].Free();

		delete [] MouseTexture;
		MouseTexture = 0;
	}
	
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Render
// Purpose: Renders the mouse cursor on screen.
// Inputs: Graphics - Pointer to the OpenGL graphics object.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool MouseObject::Render(OpenGLObject* Graphics)
{
	// Store the opengl state.
	glPushAttrib(GL_TEXTURE_BIT | GL_LIGHTING_BIT);

	// Turn on texturing and turn off lighting.
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	// Turn on alpha blending.
	glAlphaFunc(GL_GREATER, 0.2f);
	glEnable(GL_ALPHA_TEST);

	// Reset the color.
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	// Go into 2D mode.
	Graphics->OrthoMode();

	// Bind the mouse texture based on whether the mouse is clicked or not.
	mouseDown ? MouseTexture[1].Bind() : MouseTexture[0].Bind();
	
	// Render the cursor onto a quad at the position of the mouse.
	glLoadIdentity();
	glTranslated(xPos, yPos, 0);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); 
		glVertex2i(0, 32);

		glTexCoord2f(1.0f, 0.0f); 
		glVertex2i(32, 32);
				
		glTexCoord2f(1.0f, 1.0f); 
		glVertex2i(32, 0);
				
		glTexCoord2f(0.0f, 1.0f); 
		glVertex2i(0, 0);
	glEnd();
	
	// Go back to 3D mode.
	Graphics->PerspectiveMode();

	// Reset the color.
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	// Turn off alpha blending.
	glDisable(GL_ALPHA_TEST);

	// Return OpenGL to its state before entering this function.
	glPopAttrib();

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetPosition
// Purpose: Sets the position of the mouse cursor.
// Inputs: x - The x coordinate.
//         y - The y coordinate.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void MouseObject::SetPosition(int x, int y)
{
	// Store the position of the mouse.
	xPos = x;
	yPos = y;

	return;
}

int MouseObject::getX() {
	return xPos;
}

int MouseObject::getY() {
	return yPos;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetButton
// Purpose: Sets whether the left mouse button is down or not.
// Inputs: down - If the button is down or not.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void MouseObject::SetButton(bool down)
{
	mouseDown = down;
	return;
}
