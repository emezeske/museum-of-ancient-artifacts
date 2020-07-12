#include <cmath>
#include <stdio.h>
#include "renderer.h"
#include "utils.h"
using std::sin;
using std::cos;

// The window's width and height in pixels
int	SCREEN_W = 1024,
	SCREEN_H = 768;

// Definitions of static variables
vector<IsDrawable*> 	Renderer::drawList;
vector<Light*> 		Renderer::lights;

// Renderer Ctor
Renderer::Renderer() {
	// Initialize the lightCircle texture
	setNormTexMode();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenTextures(1, &lightCircle);
	loadTexture(lightCircle, "./gfx/lightCircle.tga");
	// Set up rectangular textures for use as buffers
	setNormTexMode();
	glGenTextures(1, &bumpMap);
	createBlankRectTex(bumpMap, SCREEN_W, SCREEN_H);
	glGenTextures(1, &lightMap);
	createBlankRectTex(lightMap, SCREEN_W, SCREEN_H);
	// Create normalisation cube map
	glGenTextures(1, &normCubeMap);
	genNormCubeMap(normCubeMap);
	// Default ambient lighting colors
	ambientR = 0.1;
	ambientG = 0.1;
	ambientB = 0.1;
} // end ctor Renderer()

// Iterate through the drawList and draw all the bump maps
void Renderer::DrawAllBumpMaps() {
	int size = drawList.size();
	for(int i=0; i<size; ++i)
		drawList[i]->DrawBumpMap();
} // end fnc DrawAllBumpMaps()

// Iterate through the drawList and draw all the texture maps
void Renderer::DrawAllTexMaps() {
	int size = drawList.size();
	for(int i=0; i<size; ++i)
		drawList[i]->Draw();
} // end fnc DrawAllTexMaps

// Copy the pixels in the offscreen back buffer into tex
void Renderer::GrabTexFromBackBuf(GLuint tex, int x, int y, int w, int h) {
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
	glCopyTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, x, y, w, h, 0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
} // end fnc GrabTexFromBackBuf()

// Draw all the bump maps to the back buffer and copy the back buffer into
// the bumpMap texture for later use
void Renderer::GenerateBumpMap(void) {
	// Set the back buffer pixels to vectors that point directly
	// out of the screen
	glClearColor(0.5, 0.5, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	// Draw all the bump maps to the back buffer, then copy the back
	// buffer to the bumpMap texture for texture combiner use
	glEnable(GL_TEXTURE_2D);
	setNormTexMode();
	DrawAllBumpMaps();
	glDisable(GL_TEXTURE_2D);
	GrabTexFromBackBuf(bumpMap, 0, 0, SCREEN_W, SCREEN_H);
} // end fnc GenerateBumpMap()

// A utility function that generates texture coordinates for the 4
// texture combiners and then sends the vertex down the rendering pipeline
void Renderer::DrawLitVertex(Vec3D vec, Vec3D l, float u, float v) {
	Vec3D vl = l - vec;
	// Pass the vector to light vertex to texture unit 0 for 
	// normalization lookup
	glMultiTexCoord3f(GL_TEXTURE0, vl.x, vl.y, vl.z);
	// Pass the simple texture coordinates to texture unit 1
	glMultiTexCoord2f(GL_TEXTURE1, u, v);
	// Light circle coords for texture unit 2
	/*
	float x = u/512-(l.x+256)/512;
	float diffx = x;
	if(x < -1 || x > 1)
		diffx = x - floorf(x);
	float y = v/512-(l.y+256)/512;
	float diffy = y;
	if(y < -1 || y > 1)
		diffy = y - floorf(y);
	printf("f: %f\n", diffx);
	printf("y: %f\n", diffy);
	glMultiTexCoord2f(GL_TEXTURE2, u/512-(l.x+256)/512, v/512-(l.y+256)/512);
	*/
	// Pass the simple texture coordinates to texture unit 3
	glMultiTexCoord2f(GL_TEXTURE2, u, v);
	// Render this vertex
	glVertex3f(vec.x, vec.y, vec.z);
} // end fnc DrawLitVertex()

// This is the heart of the rendering engine.  This function sets up the 4
// texture combiners to do as follows: 
// 	1) Create a texture that contains the normalized vectors from
// 		the current light to the current texel
// 	2) Take the dot product of the current bumpMap texel and the
// 		current texel from the previous texture combiner
// 	3) Multiply the previous texture combiner's output with a
// 		luminosity map that makes the light look like a point
// 		light, spotlight, etc
// 	4) Multiply the previous texture combiner's output with the current
// 		light's color.
// Each of these steps must be performed for each light.  After all four steps
// are completed, each light is rendered to the back buffer additively, and then
// finally the back buffer is copied into the lightMap texture for later use.
void Renderer::GenerateLightMap(void) {
	// Texture unit 0 -- the cubemap vector normalizer
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, normCubeMap);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
	// Set up texture unit 1 -- the unit that performs the dot
	// product on the normal map and the bump map
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, bumpMap);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_DOT3_RGB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
	// Texture unit 2 controls the luminosity texture
	/*
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, lightCircle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
	*/
	// Texture unit 3 lets us change colors and intensities of lights
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, lightMap);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_CONSTANT);

	glClearColor(ambientR, ambientG, ambientB, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// If the combined intensity of the lights exceeds 1.0, it needs to be
	// clamped to 1.0.  This adds up their intensities.
	float totalAlpha = 0;
	for(int i=0; i<lights.size(); ++i)
		totalAlpha += lights[i]->i;
	
	// Now that everything is set up, render all the lights!!
	for(int i=0; i<lights.size(); ++i) {
		printf("\n");
		GLfloat c[4] = { lights[i]->r, lights[i]->g, lights[i]->b, lights[i]->i};
		// Set the GL_CONSTANT color for combination in texture unit 3
		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, c);

		glBegin(GL_QUADS);
			DrawLitVertex(Vec3D(0,		0,		0), lights[i]->pos,  0,		0);
			DrawLitVertex(Vec3D(0,		SCREEN_H,	0), lights[i]->pos,  0,		SCREEN_H);
			DrawLitVertex(Vec3D(SCREEN_W, 	SCREEN_H,	0), lights[i]->pos,  SCREEN_W,	SCREEN_H);
			DrawLitVertex(Vec3D(SCREEN_W,	0,		0), lights[i]->pos,  SCREEN_W,	0);
		glEnd();
	}
	// Undo the madness that is multiple texture units
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_CUBE_MAP);
	// Copy the newly constructed lightmap into the tex lightMap
	GrabTexFromBackBuf(lightMap, 0, 0, SCREEN_W, SCREEN_H);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
} // end GenerateLightMap()

// This function pieces together the parts of the rendering engine
// into the actual output that should go to the screen.
void Renderer::DrawScene() {
	// Generate the bump map (GenerateLightMap uses it)
	GenerateBumpMap();
	// Generate the light map (To be added to the finished product)
	GenerateLightMap();
	setNormTexMode();

	// Draw everything's actual texture maps to the back buffer
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	DrawAllTexMaps();
	glDisable(GL_TEXTURE_2D);

	// Add the lightmap to the back buffer
	glBlendFunc(GL_ONE, GL_SRC_COLOR);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, lightMap);
	glBegin(GL_QUADS);
		glTexCoord2f(	0	, 0);
		glVertex2f(	0	, 0);
		glTexCoord2f(	0	, SCREEN_H);
		glVertex2f(	0	, SCREEN_H);
		glTexCoord2f(	SCREEN_W, SCREEN_H);
		glVertex2f(	SCREEN_W, SCREEN_H);
		glTexCoord2f(	SCREEN_W, 0);
		glVertex2f(	SCREEN_W, 0);
	glEnd();
	// Cleanup
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
} // end fnc DrawScene()

// Merely for testing, this code rotates one of the lights
// around in a circle and changes another of the light's colors
void Renderer::WiggleLights(int delta) {
	static float phase = 0.0;
	phase += 0.003 * delta;
	if(phase > 2*M_PI)
		phase = phase-2*M_PI;
	lights[0]->pos.x = cos(phase) * 128 + 256;
	lights[0]->pos.y = sin(phase) * 128 + 256;
	lights[2]->r = 0.5*cos(phase)+0.5;
	lights[2]->g = 0.5*cos(phase+2*M_PI/3)+0.5;
	lights[2]->b = 0.5*cos(phase+4*M_PI/3)+0.5;
} // end fnc WiggleLights()
