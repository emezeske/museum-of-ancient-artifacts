
#ifndef __GWORLD_H__
#define __GWORLD_H__

#include <GL/gl.h>
#include <stdio.h>

#include "ltga.h"
#include "vector.h"

typedef struct Texture2 {
	float u, v;
	Texture2(float U, float V): u(U), v(V) { }
	Texture2(): u(0.0), v(0.0) {}
} Texture2;

typedef struct VERTEX {
	Vector3 pos;
	Texture2 tex;
} VERTEX;

class gWorld;			// because wQuad needs a gWorld *

class wQuad {
	public:
	VERTEX vertex[4];
	Vector3 normal;
	wQuad *next, *prev;
	static gWorld *world;
	int texture;
	void Draw();				// draw w/ texture to gl context
	void Sketch();				// draw outline as line strip
	void CalculateNormal();	
	void InvertNormal();			// reverse vertex order and CalculateNormal()
	void Move(Vector3 &);
	void SetVertices(Vector3 *);
	void SetTextureCoords(Texture2 *);
	void SetTexture(int);
}; // end class wQuad

class wSector {
	public:
	int polygons;
	wQuad *quads;
	wSector *next;
	wSector();
	void AddQuad(wQuad *);
	void DeleteQuad(wQuad *);
}; // end class wSector

class gWorld {
	public:
	int numtextures, numheightmaps, numterrains;
	wSector *sectors;
	GLuint *texture;
	int topSBtx, sidesSBtx;
	
	gWorld(char *filename);
	~gWorld();
	char textureFilename[256][32];
	void renderScene();
	void renderSketch();
	void loadPolyFromFile(wQuad *);
	void loadFromFile(char *filename);
	void writePolyToFile(wQuad *);
	void writeToFile(char *filename);
	FILE *fileIn, *fileOut;

}; // end class gWorld

#endif

