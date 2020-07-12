///////////////////////////////////////////////////////////////////////////////
// Filename: quadtreeobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _QUADTREEOBJECT_H_
#define _QUADTREEOBJECT_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "terrainobject.h"
#include "vertexbufferobject.h"
#include "frustumobject.h"


///////////////////////////////////////////////////////////////////////////////
// Class name: QuadTreeObject
///////////////////////////////////////////////////////////////////////////////
class QuadTreeObject
{
private:
	struct PolygonType
	{
	unsigned int textureGroup;
        unsigned long timeLastDrawn;
        unsigned int verIndex[3];
        unsigned int texIndex[3];
        unsigned int norIndex[3];

		PolygonType()
		{ 
			textureGroup = 0;
			timeLastDrawn = 0;
		}
    };

	struct GroupType
	{
		VertexBufferObject VertexBuffer;
		unsigned int numPolygons;
		unsigned int numPolygonsToDraw;

		GroupType()
		{ 
			numPolygons = 0;     
		}

		~GroupType() 
		{ 
			VertexBuffer.Free(); 
		}
	};

	struct NodeType
	{
		unsigned int numPolygons;
        PolygonType** polygonList;
        float xPos, yPos, zPos, size;
        NodeType* nodes[4];

		NodeType()
		{
			numPolygons = 0;
			polygonList = NULL;
			xPos = yPos = zPos = size = 0.0f;
			
			for(short i=0; i<4; i++)
			{
				nodes[i] = NULL;
			}
		}

		~NodeType()
		{
			if(polygonList)
			{
				delete [] polygonList;
				polygonList = NULL;
			}

			for(short i=0; i<4; i++)
			{
				if(nodes[i])
				{
					delete nodes[i];
					nodes[i] = NULL;
				}
			}
		}
	};

	struct VertexType 
	{ 
		float x, y, z; 
	};

public:
	QuadTreeObject();
	QuadTreeObject(const QuadTreeObject&);
	~QuadTreeObject();

	bool Initialize(TerrainObject*);
	bool Shutdown();
	bool Render(FrustumObject*);

	unsigned int GetPolysDrawn();
	bool GetHeightAtPosition(float, float, float&);

private:
	void SortNode(NodeType*, float, float, float, float);
	unsigned int CountPolygons(float, float, float, float);
	bool IsPolygonContained(PolygonType*, float, float, float, float);
	void AddNode(NodeType*);
	void FindNode(NodeType*, float, float, float&);
	bool FoundHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);

private:
	TerrainObject* Terrain;         // The terrain mesh.
	float mMeshCubeSize;            // The maximum radius of the terrain mesh.
	float mMaxNodeSize;             // Maximum physical size of a node.
	unsigned int mMaxNodePolygons;  // Maximum number of polygons in a node.
	unsigned int mNumPolygons;      // Number of polygons in terrain mesh.
	PolygonType* mPolygonList;      // List of indexes into the terrain mesh.
	unsigned int mNumTexGroups;     // Number of textures in the terrain mesh.
	GroupType* mTextureGroups;      // List of textures in the terrain mesh.
	NodeType* mParentNode;          // The parent node for the quad tree.
	FrustumObject* FrustumPtr;      // Pointer to the frustum object.
	unsigned long currentTime;      // The current time of the render.
	unsigned int mTotalPolysDrawn;  // The number of polys drawn per scene.
	float meshMinX, meshMaxX;       // The x perimeter of the terrain mesh.
	float meshMinZ, meshMaxZ;		// The z perimeter of the terrain mesh.
};

#endif
