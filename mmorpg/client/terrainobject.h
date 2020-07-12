///////////////////////////////////////////////////////////////////////////////
// Filename: terrainobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _TERRAINOBJECT_H_
#define _TERRAINOBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <GL/glut.h>
#include <fstream>
#include <math.h>

using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureobject.h"


///////////////////////////////////////////////////////////////////////////////
// Class name: TerrainObject
///////////////////////////////////////////////////////////////////////////////
class TerrainObject
{
private:
	struct VertexType 
	{ 
		float x, y, z; 
	};

	struct TexCoordType 
	{ 
		float tu, tv; 
	};

	struct NormalType 
	{ 
		float nx, ny, nz; 
	};

	struct FaceType
	{
		unsigned int vIndex1, vIndex2, vIndex3;
		unsigned int tIndex1, tIndex2, tIndex3;
		unsigned int nIndex1, nIndex2, nIndex3;
	};

public:
	TerrainObject();
	TerrainObject(const TerrainObject&);
	~TerrainObject();

	bool Initialize(char*);
	bool Shutdown();

	unsigned int GetNumFaces();
	void GetVertexIndex(unsigned int, unsigned int&, unsigned int&, unsigned int&);
	void GetTextureIndex(unsigned int, unsigned int&, unsigned int&, unsigned int&);
	void GetNormalIndex(unsigned int, unsigned int&, unsigned int&, unsigned int&);
	unsigned int GetTextureIndex(unsigned int);
	unsigned int GetTextureCount();
	void GetDimensions(float&, float&, float&, float&);
	void GetVertexData(unsigned int, float&, float&, float&);
	void GetTextureCoordinateData(unsigned int, float&, float&);
	void GetNormalData(unsigned int, float&, float&, float&);
	void SetTexture(unsigned int);
	void GetPerimeter(float&, float&, float&, float&);

private:
	TextureObject* GroundTexture;
	VertexType* Vertices;
	TexCoordType* TexCoords;
	NormalType* Normals;
	FaceType* Faces;
	unsigned int width, height, numVerts, numNorms, numFaces;
	float center[3];
	float terrainDiameter;
	float minimumX, maximumX, minimumZ, maximumZ;
};

#endif
