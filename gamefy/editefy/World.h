#ifndef __EDITEFYWORLD_H__
#define __EDITEFYWORLD_H__

#include <vector>
#include <GL/glut.h>
#include "../shared/ltga.h"
#include "Object3D.h"
#include "selection.h"

class Object3D;

class StaticObject {
	public:
	bool active;
	unsigned short model_indx;
	float x,  y,  z,
	      xr, yr, zr;
	void Draw();
	void Sketch();
}; // end class StaticObject

class Material {
	public:
	char *name;
	char *filename;
	GLuint tex_num;
}; // end class Material

class EditefyWorld {
	public: 
	// The list of all the materials (textures)
	std::vector<Material> materials;
	// The command-line supplied level
	Object3D *levelModel;
	// A list of the objects in the /obj folder
	unsigned short num_models;
	Object3D *objectModels;
	// The list of objects that are in the world
	std::vector<StaticObject> static_objects;

	void LoadLevel(const char *);
	void LoadMaterials();
	void AddStaticObject(unsigned short);
	void RemoveStaticObject(unsigned short);
	void RenderScene();
	void RenderSketch();
}; // end class EditefyWorld

#endif
