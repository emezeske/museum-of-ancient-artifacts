#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include <GL/glew.h>
#include <GL/glut.h>
#include "isdrawable.h"

// This struct is temporary;  it just draws a bumpmapped background to the 
// screen for testing the lighting and so on.  It also demonstrates how to 
// use the IsDrawable interface.
class BackGround : public IsDrawable {
	private:
		// Width/Height of the background texture
		static const int texSize = 512;
		// The two texture labels for the GL
		// tex is the RGB texture and bmtex is the bumpmap texture
		GLuint tex, bmtex;
		// This function simply takes a texture and draws it across
		// the whole screen
		void DrawFullScreenQuad(GLuint);
	public:
		// Default ctor
		BackGround();
		// These are the functions that must be implemented to use the
		// IsDrawable interface  
		virtual void DrawBumpMap();
		virtual void Draw();
};
#endif
