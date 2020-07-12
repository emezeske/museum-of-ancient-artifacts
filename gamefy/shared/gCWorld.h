#ifndef __GLEVEL_H__
#define __GLEVEL_H__

#include <GL/gl.h>
#include "../shared/vector.h"
#include "../shared/gWorld.h"

#define		MAX_POLYS_PER_NODE	3
#define		MAX_OCTAL_TREE_DEPTH	4

struct AaBox {			// axis-aligned bounding box
	float	xmin, xmax,
		ymin, ymax,
		zmin, zmax;
};

struct Sphere {
	Vector3 c;
	float radius, radiusSq;
};

struct CWorldHeader {
	short nSectors;
	short nTextures;
	char textureFilename[256][32];
};

struct CSectorHeader {
	AaBox		boundingBox;
	Sphere		boundingSphere;
	unsigned int	nTriangles;
	unsigned char	nPortals;
};

struct CSectorPortal {
	unsigned short	idOppositeSector;
//	nSidedPoly	polygon;
};

enum NodeResult {
	NODE_EMPTY,		// the node contains 0 polygons
	NODE_END,		// the node contains few enough polygons to consider it the end node
	NODE_SUBDIVIDED		// the node contains enough polygons to be broken apart into subnodes
};

class Tri {
	public:
	VERTEX vertex[3];
	int texture;
	void Draw();
}; // end class Tri

class Quad {
	public:
	VERTEX vertex[4];
	int texture;
}; // end class Quad

class gCWorld;

class OctalTree {
	public:
	OctalTree	*_000, *_001, *_010, *_011,	// _XYZ
			*_100, *_101, *_110, *_111;
	Sphere boundingSphere;
	AaBox  boundingBox;		// axis-aligned
	Tri *tri;			// triangle array
	int tris;
	static unsigned char recursionLevel;
	static unsigned short nodes;
	static gCWorld *lvl;
	GLuint list;			// the glList id of this node's shit
	static GLuint curList;		// temporary for adding lists
	unsigned int *triangleIndeces;
	bool vertexContained(Vector3);
	bool polygonContained(Tri *);
	int numberOfTrisContained(Tri *, int);
	int numberOfTrisOwned(Tri *, int);
	void AddBoundedTriangles(Tri *, int);
	void writeToFile(FILE *);
	void readFromFile(FILE *);
	void writeOrNull(OctalTree *, FILE *);
	void drawSelfAndMaybeChildren(bool);	// draw all this node's tris and either test children or draw them all
	void testDraw(bool);	// test and draw if necessary
	void draw();		// draw all triangles in this node
	void compileList();
	OctalTree * readOrNull(FILE *);
	OctalTree();
	OctalTree(float, float, float, float, gCWorld *);
	~OctalTree();
}; // end class OctalTree

// gamefy compiled world
class gCWorld {
	public:
	int numtextures; 
	GLuint * texture; 
	int nPolys;
	OctalTree *treeRoot;
	GLuint drawLists;
	NodeResult buildTree(OctalTree *, Tri *, int);
	void writeToFile(FILE *);
	void readFromFile(FILE *);
	void loadTextures(const CWorldHeader &);
	void renderSkybox();
	void renderScene();
	gCWorld();
	gCWorld(const char *filename);
	gCWorld(int numtextures, int nPolys, Tri *polys);
	~gCWorld();
}; // end class gLevel

#endif
