///////////////////////////////////////////////////////////////////////////////
// Filename: openglobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "openglobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: OpenGLObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
OpenGLObject::OpenGLObject()
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: OpenGLObject
// Purpose: Copy constructor.
// Inputs: other - The object this function is making a copy of.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
OpenGLObject::OpenGLObject(const OpenGLObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~OpenGLObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
OpenGLObject::~OpenGLObject()
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Initialize
// Purpose: Initializes OpenGL for use by the program.
// Inputs: Hwnd - Handle to the window.
//         width - The width of the screen.
//         height - The height of the screen.
//         sDepth - The far plane of the scene.
//         sNear - The near plane of the scene.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool OpenGLObject::Initialize(int width, int height, float sDepth,  float sNear)
{

	// Save the screen dimensions.
	screenWidth  = width;
	screenHeight = height;
	screenDepth  = sDepth;
	screenNear   = sNear;

	// Setup the viewport.
	glViewport(0, 0, screenWidth, screenHeight);						

	// Reset projection matrix.
	glMatrixMode(GL_PROJECTION);						
	glLoadIdentity();									

	// Calculate aspect ratio of window.
	gluPerspective(45.0f, (GLfloat)screenWidth / (GLfloat)screenHeight, screenNear, screenDepth);

	// Reset modelview matrix.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Drawing settings.
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDepthRange(sNear, sDepth);
	glClearDepth(1.0f);									
	glEnable(GL_DEPTH_TEST);							
	glDepthFunc(GL_LEQUAL);								
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	lineMode = false;

	// Turn on polygon face culling so back faces are not drawn.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Turn on lighting.
	glEnable(GL_LIGHTING);

	// Turn on texturing.
	glEnable(GL_TEXTURE_2D);

	// Clear the screen to a blue color.
	glClearColor(0.3f, 0.6f, 0.8f, 0.0f);

	// Set the render mode to normal.
	renderMode = 1;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Shutdown
// Purpose: Shuts down OpenGL.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool OpenGLObject::Shutdown()
{
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: BeginScene
// Purpose: Begins a rendering scene.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void OpenGLObject::BeginScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: EndScene
// Purpose: Ends a rendering scene and displays to the screen.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void OpenGLObject::EndScene()
{
	glutSwapBuffers();
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: OrthoMode
// Purpose: Changes the drawing mode into orthographics mode.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void OpenGLObject::OrthoMode()
{
	// Disable depth testing so text is on the top.
	glDisable(GL_DEPTH_TEST);     

	// Go into projection mode.
	glMatrixMode(GL_PROJECTION);  

	// Save the projection matrix.
	glPushMatrix();

	// Load the identity matrix.
	glLoadIdentity();

	// Setup orthographic mode.
	glOrtho(0, screenWidth, screenHeight, 0, -1, 1);

	// Go into model view mode.
	glMatrixMode(GL_MODELVIEW);

	// Save the model view matrix.
	glPushMatrix();

	// Load the identity matrix.
	glLoadIdentity();

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: PerspectiveMode
// Purpose: Changes the drawing mode into perspective mode.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void OpenGLObject::PerspectiveMode()
{
	// Go into projection mode.
	glMatrixMode(GL_PROJECTION);

	// Restore the projection matrix.
	glPopMatrix();

	// Go into model view mode.
	glMatrixMode(GL_MODELVIEW);					

	// Restore the model view matrix.
	glPopMatrix();

	// Re-enable depth testing.
	glEnable(GL_DEPTH_TEST);

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ChangeRenderMode
// Purpose: Changes to a different rendering mode.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void OpenGLObject::ChangeRenderMode()
{
	// Increment the render mode to the next one.
	renderMode++;
	if(renderMode == 8)
	{
		renderMode = 1;
	}

	// Switch the rendering mode.
	switch(renderMode)
	{
		// Normal rendering.
		case 1:
		{
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT, GL_FILL);
			glPolygonMode(GL_BACK,  GL_FILL);
			lineMode = false;

			glEnable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			break;
		}

		// Normal with no lighting.
		case 2:
		{
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT, GL_FILL);
			glPolygonMode(GL_BACK,  GL_FILL);
			lineMode = false;

			glDisable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			break;
		}

		// No texture rendering mode.
		case 3:
		{
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT, GL_FILL);
			glPolygonMode(GL_BACK,  GL_FILL);
			lineMode = false;

			glEnable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			break;
		}

		// Line mode.
		case 4:
		{
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT, GL_LINE);
			glPolygonMode(GL_BACK,  GL_LINE);
			lineMode = true;

			glEnable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			break;
		}

		// Line mode with no texture colors on the lines.
		case 5:
		{
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT, GL_LINE);
			glPolygonMode(GL_BACK,  GL_LINE);
			lineMode = true;
			
			glEnable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			break;
		}

		// Line mode with no texturing and no culling.
		case 6:
		{
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT, GL_LINE);
			glPolygonMode(GL_BACK,  GL_LINE);
			lineMode = true;
			
			glEnable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			break;
		}

		// Line mode with no lighting, texturing, or culling.
		case 7:
		{
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT, GL_LINE);
			glPolygonMode(GL_BACK,  GL_LINE);
			lineMode = true;

			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			break;
		}

		// Default is normal rendering.
		default:
		{
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT, GL_FILL);
			glPolygonMode(GL_BACK,  GL_FILL);
			lineMode = false;

			glEnable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			break;
		}
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: InLineMode
// Purpose: Lets the caller know if the render mode is a line mode.
// Inputs: None.
// Outputs: lineMode - If it is in line mode or not.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool OpenGLObject::InLineMode()
{
	return lineMode;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetRenderMode
// Purpose: Lets the caller know which render mode it is in.
// Inputs: None.
// Outputs: renderMode - The current render mode.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
int OpenGLObject::GetRenderMode()
{
	return renderMode;
}


