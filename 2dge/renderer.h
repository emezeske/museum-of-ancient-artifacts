#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include "vec3d.h"
#include "isdrawable.h"
#include "light.h"
using std::vector;

// The renderer!  This is the class that provides all the functionality
// needed to render the scene
class Renderer {
	private:
		// List of pointers to all the objects that are to be drawn
		static vector<IsDrawable*> drawList;
		// The list of pointers to all the lights to be rendered
		static vector<Light*> lights;
		// Draws all the bump maps to the back buffer
		void DrawAllBumpMaps();
		// Draws all the texture maps to the back buffer
		void DrawAllTexMaps();
		// Copies the back buffer into a texture
		void GrabTexFromBackBuf(GLuint, int, int, int, int);
		// Draws the whole scene's bump map and makes a texture out of it
		void GenerateBumpMap();
		// Util function for GenerateLightMap()
		void DrawLitVertex(Vec3D, Vec3D, float, float);
		// Construct the lightmap that will be added to the scene
		void GenerateLightMap();
		GLuint	bumpMap,	// contains the fully constructed bumpmap
			lightMap,	// contains the fully constructed lightmap
			lightCircle,	// contains the luminosity texture
			normCubeMap;	// contains the normalization cube map
		// The ambient lighting color
		float ambientR, ambientG, ambientB;
	public:
		// Default ctor
		Renderer();
		// Take care of the whole rendering process
		void DrawScene();
		// Move the lights around and stuff to make sure they work correctly
		void WiggleLights(int);
		// Necessary so that the IsDrawable interface can access
		// private member drawList
		friend class IsDrawable;
		// Light class needs access too
		friend class Light;
}; // end class Renderer
#endif
