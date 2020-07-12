///////////////////////////////////////////////////////////////////////////////
// Filename: openglobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _OPENGLOBJECT_H_
#define _OPENGLOBJECT_H_

//////////////
// INCLUDES //
//////////////
#include <GL/glut.h>

///////////////////////////////////////////////////////////////////////////////
// Class name: OpenGLObject
///////////////////////////////////////////////////////////////////////////////
class OpenGLObject
{
public:
	OpenGLObject();
	OpenGLObject(const OpenGLObject&);
	~OpenGLObject();

	bool Initialize(int, int, float, float);
	bool Shutdown();

	void BeginScene();
	void EndScene();

	void OrthoMode();        // 2D mode.
	void PerspectiveMode();  // 3D mode.

	void ChangeRenderMode();  // Changes the render mode to the next mode.
	bool InLineMode();        // Lets caller know if in a line render mode.
	int  GetRenderMode();     // Gives the current render mode.

private:
	int screenWidth;    // The width of the screen resolution.
	int screenHeight;   // The height of the screen resolution.
	float screenDepth;  // The far plane of the scene.
	float screenNear;   // The near plane of the scene.
	bool lineMode;      // If the rendering is in line mode or poly mode.
	int renderMode;     // The current rendering mode.
};

#endif
