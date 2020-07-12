#ifndef __OBJECT3D_H__
#define __OBJECT3D_H__

#include <stdio.h>
#include <sys/stat.h>
#include <GL/glut.h>
#include "World.h"

// Some defines for reading the .3ds file
#define MAIN_CHUNK 	0x4d4d
#define ED_3D_CHUNK 	0x3d3d
#define OBJ_CHUNK 	0x4000
#define TRI_CHUNK 	0x4100
#define VERT_CHUNK 	0x4110
#define FACES_CHUNK 	0x4120
#define FACE_MAT_CHUNK	0x4130
#define TEX_COORD_CHUNK 0x4140
#define ED_MAT_CHUNK	0xAFFF
#define MAT_NAME_CHUNK  0xA000
#define MAT_TEX1_CHUNK	0xA200
#define MAT_FNAME_CHUNK	0xA300

class Vertex {
	public:
	float 	x, y, z,
		u, v;
}; // end class Vertex

class Triangle {
	public:
	int vert_indxs[3];
	int material_num;
}; // end class Triangle

class Object3D {
	unsigned short num_vertices;
	Vertex *vertices;
	unsigned short num_tris;
	Triangle *tris;

	char *name;

	public:
	// Initialize the Object3D with the name of a .3ds file
	Object3D(const char *);
	void Draw();
	void Sketch();
	~Object3D();
}; // end class Object3D

#endif
