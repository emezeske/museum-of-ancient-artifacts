///////////////////////////////////////////////////////////////////////////////
// Filename: staticmodelobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _STATICMODELOBJECT_H_
#define _STATICMODELOBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <GL/glut.h>
#include <fstream>
#include <math.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureobject.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// Class: StaticModelObject
///////////////////////////////////////////////////////////////////////////////
class StaticModelObject
{
private:
	struct VertType
	{
		float x, y, z;
	};

	struct TexType
	{
		float tu, tv;
	};

	struct NorType
	{
		float nx, ny, nz;
	};

	struct FaceType
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
		int textureGroup;
	};

	struct TextureGroupType
	{
		int numFaces;
		int numElements;
		float* dataBuffer;
		TextureObject* Texture;
	};

public:
	StaticModelObject();
	StaticModelObject(const StaticModelObject&);
	~StaticModelObject();

	bool Initialize(char*);
	bool Shutdown();
	void Render();

	void SetPosition(float, float, float);
	int  GetPolyCount();
	void GetBoundingVolume(float&, float&, float&, float&);

private:
	TextureGroupType* TextureGroups;
	int groupCount;
	float xPos, yPos, zPos;
	float centerX, centerY, centerZ, cubeSize;
	int numPolys;
};

#endif
