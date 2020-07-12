#ifndef __UTILS_H__
#define __UTILS_H__
#include <GL/glut.h>

// Generate the normalization cube map
extern void genNormCubeMap(GLuint);
// Set the texture parameters to defaults
extern void setNormTexMode(void);
// Load a *.tga image into an OpenGL texture
extern void loadTexture(GLuint, const char*);
// Initialize a texture with no image information
extern void createBlankTexture(GLuint, int, int, bool);
// Initialize a texture with no image information that does not have to
// be of width or height 2^N
extern void createBlankRectTex(GLuint, int, int);

#endif
