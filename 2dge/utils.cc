#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include "vec3d.h"
#include "ltga.h"
///////////////////////////////////////////////////////////////////////////
// UTILITY FUNCTIONS
////////////////////////////////////////////////////////////////////////

// This monstrosity of a mangled function sets up a normalization cube map.  
// Setting up this map is complicated but it allows the renderer to normalize
// vectors (3D) extremely quickly, basically using a weird type of look-up table.
void genNormCubeMap(GLuint normCubeMap) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, normCubeMap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	const int cubeSize 		= 64;
	const float halfCubeSize 	= (float)cubeSize/2;
	unsigned char* data, *p_data;
	// POSITIVE X FACE OF CUBE
	data = new unsigned char[cubeSize*cubeSize*3];
	p_data = data;
	for(float i = -halfCubeSize+0.5; i < halfCubeSize; i+=1.0)
		for(float j = -halfCubeSize+0.5; j < halfCubeSize; j+=1.0) {
			Vec3D t(halfCubeSize, -i, -j);
			// Pack into one byte and save
			t.Clamp01();
			p_data[0] = (unsigned char)(t.x*255);
			p_data[1] = (unsigned char)(t.y*255);
			p_data[2] = (unsigned char)(t.z*255);
			p_data += 3;
		}
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			0, GL_RGB8, cubeSize, cubeSize, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// NEGATIVE X FACE OF CUBE
	p_data = data;
	for(float i = -halfCubeSize+0.5; i < halfCubeSize; i+=1.0)
		for(float j = -halfCubeSize+0.5; j < halfCubeSize; j+=1.0) {
			Vec3D t(-halfCubeSize, -i, j);
			// Pack into one byte and save
			t.Clamp01();
			p_data[0] = (unsigned char)(t.x*255);
			p_data[1] = (unsigned char)(t.y*255);
			p_data[2] = (unsigned char)(t.z*255);
			p_data += 3;
		}
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			0, GL_RGB8, cubeSize, cubeSize, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// POSITIVE Y FACE OF CUBE
	p_data = data;
	for(float i = -halfCubeSize+0.5; i < halfCubeSize; i+=1.0)
		for(float j = -halfCubeSize+0.5; j < halfCubeSize; j+=1.0) {
			Vec3D t(j, halfCubeSize, i);
			// Pack into one byte and save
			t.Clamp01();
			p_data[0] = (unsigned char)(t.x*255);
			p_data[1] = (unsigned char)(t.y*255);
			p_data[2] = (unsigned char)(t.z*255);
			p_data += 3;
		}
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			0, GL_RGB8, cubeSize, cubeSize, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// NEGATIVE Y FACE OF CUBE
	p_data = data;
	for(float i = -halfCubeSize+0.5; i < halfCubeSize; i+=1.0)
		for(float j = -halfCubeSize+0.5; j < halfCubeSize; j+=1.0) {
			Vec3D t(j, -halfCubeSize, -i);
			// Pack into one byte and save
			t.Clamp01();
			p_data[0] = (unsigned char)(t.x*255);
			p_data[1] = (unsigned char)(t.y*255);
			p_data[2] = (unsigned char)(t.z*255);
			p_data += 3;
		}
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			0, GL_RGB8, cubeSize, cubeSize, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// POSITIVE Z FACE OF CUBE
	p_data = data;
	for(float j = -halfCubeSize+0.5; j < halfCubeSize; j+=1.0)
		for(float i = -halfCubeSize+0.5; i < halfCubeSize; i+=1.0) {
			Vec3D t(i, -j, halfCubeSize);
			// Pack into one byte and save
			t.Clamp01();
			p_data[0] = (unsigned char)(t.x*255);
			p_data[1] = (unsigned char)(t.y*255);
			p_data[2] = (unsigned char)(t.z*255);
			p_data += 3;
		}
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			0, GL_RGB8, cubeSize, cubeSize, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// NEGATIVE Z FACE OF CUBE
	p_data = data;
	for(float j = -halfCubeSize+0.5; j < halfCubeSize; j+=1.0)
		for(float i = -halfCubeSize+0.5; i < halfCubeSize; i+=1.0) {
			Vec3D t(-i, -j, -halfCubeSize);
			// Pack into one byte and save
			t.Clamp01();
			p_data[0] = (unsigned char)(t.x*255);
			p_data[1] = (unsigned char)(t.y*255);
			p_data[2] = (unsigned char)(t.z*255);
			p_data += 3;
		}
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
			0, GL_RGB8, cubeSize, cubeSize, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	delete [] data;
}

// This set of functions had to get called together so much that they are
// a single function now.  They basically set up the standard texture modes.
void setNormTexMode(void) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
} // end fnc setNormTexMode()

// Load a *.tga texture from a file into an OpenGL texture
void loadTexture(GLuint tex, const char* file) {
	LTGA l;
	if(!l.LoadFromFile(file)) {
		printf("FATAL: Failed to load texture file: %s\n", file);
		exit(-1);
	}
	glBindTexture(GL_TEXTURE_2D, tex);
	int format = (l.GetAlphaDepth() == 8) ? GL_RGBA : GL_RGB;
	glTexImage2D (GL_TEXTURE_2D,
		      0,
		      format,
		      l.GetImageWidth(),
		      l.GetImageHeight(),
		      0,
		      format,
		      GL_UNSIGNED_BYTE,
		      l.GetPixels() );
} // end loadTextures()

// Creates an OpenGL texture of width w and height h, with or without
// an alpha channel, and does not initialize it
void createBlankTexture(GLuint tex, int w, int h, bool hasAlpha) {
	glBindTexture(GL_TEXTURE_2D, tex);
	setNormTexMode();
	int format = (hasAlpha) ? GL_RGBA : GL_RGB;
	glTexImage2D (GL_TEXTURE_2D,
		      0,
		      format,
		      w,
		      h,
		      0,
		      format,
		      GL_UNSIGNED_BYTE,
		      NULL);
} // end fnc createBlankTexture()

// Creates an OpenGL texture of width w and height h, with no 
// alpha channel, and does not initialize it.  This is ONLY for textures
// that do not have sides of length 2^N.  This is only useful when one needs
// to save texture memory, and probably incurs a slight performance hit.
void createBlankRectTex(GLuint tex, int w, int h) {
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
	setNormTexMode();
	glTexImage2D (GL_TEXTURE_RECTANGLE_ARB,
		      0,
		      GL_RGB,
		      w,
		      h,
		      0,
		      GL_RGB,
		      GL_UNSIGNED_BYTE,
		      NULL);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
} // end fnc createBlankRectTex()
