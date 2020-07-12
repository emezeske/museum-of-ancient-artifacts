///////////////////////////////////////////////////////////////////////////////
// Filename: quadtreeobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "quadtreeobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: QuadTreeObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Initializes the member variables of the object to null.
///////////////////////////////////////////////////////////////////////////////
QuadTreeObject::QuadTreeObject()
{
	mPolygonList =   0;
	mTextureGroups = 0;
	mParentNode =    0;
	Terrain =        0;
	FrustumPtr =     0;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: QuadTreeObject
// Purpose: Copy constructor.
// Inputs: other - The object this function is making a copy of.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
QuadTreeObject::QuadTreeObject(const QuadTreeObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~QuadTreeObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
QuadTreeObject::~QuadTreeObject()
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Initialize
// Purpose: Creates the quad tree from the given mesh.
// Inputs: TerrainMesh - The terrain mesh we are generating the tree from.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool QuadTreeObject::Initialize(TerrainObject* TerrainMesh)
{
	unsigned int i;
	float centerX, centerY, centerZ;


	// Save a pointer to the terrain mesh.
	Terrain = TerrainMesh;

	// Set the maximum physical size of a quadtree node.
	mMaxNodeSize = 256.0f;

	// Set the maximum number of polygons in a node.
	mMaxNodePolygons = 128;

	// Get the number of polygons in the terrain mesh.
	mNumPolygons = Terrain->GetNumFaces();

	// Create a list of the polygon indexes into the terrain mesh.
	mPolygonList = new PolygonType[mNumPolygons];

	// Get the number of texture groups in the mesh.
	mNumTexGroups = Terrain->GetTextureCount();

	// Create the texture groups list.
	mTextureGroups = new GroupType[mNumTexGroups];

	// Go through all the polygons in the terrain and retrieve their indexes.
	for(i=0; i<mNumPolygons; i++)
	{
		// Get the list of vertex, texture coordinate, and normal indexes from 
		// the terrain mesh.
		Terrain->GetVertexIndex(i, mPolygonList[i].verIndex[0], mPolygonList[i].verIndex[1],
								mPolygonList[i].verIndex[2]);
		
		Terrain->GetTextureIndex(i, mPolygonList[i].texIndex[0], mPolygonList[i].texIndex[1],
								 mPolygonList[i].texIndex[2]);

		Terrain->GetNormalIndex(i, mPolygonList[i].norIndex[0], mPolygonList[i].norIndex[1],
								mPolygonList[i].norIndex[2]);

		// Get the texture index for this polygon.
		mPolygonList[i].textureGroup = Terrain->GetTextureIndex(i);

		// Increment the count for that texture group.
		mTextureGroups[mPolygonList[i].textureGroup].numPolygons++;
	}

	// Create the texture group vertex buffers.
	for(i=0; i<mNumTexGroups; i++)
	{
		if(mTextureGroups[i].numPolygons > 0)
		{
			mTextureGroups[i].VertexBuffer.Create(mTextureGroups[i].numPolygons * 3);
		}
	}

	// Retrieve the dimensions of the terrain mesh.
	Terrain->GetDimensions(centerX, centerY, centerZ, mMeshCubeSize);

	// Retrieve the perimeter of the terrain mesh.
	Terrain->GetPerimeter(meshMinX, meshMaxX, meshMinZ, meshMaxZ);

	// Create the parent node for the quad tree.
	mParentNode = new NodeType;

	// Sort all the polygons into nodes starting with the parent node at the 
	// center.
	SortNode(mParentNode, centerX, centerY, centerZ, mMeshCubeSize);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Shutdown
// Purpose: Shuts down the quad tree.
// Inputs: None.
// Outputs: None.
// Details: Releases all the memory associated with this object.
///////////////////////////////////////////////////////////////////////////////
bool QuadTreeObject::Shutdown()
{
	Terrain = 0;
	FrustumPtr = 0;

	if(mParentNode)
	{
		delete mParentNode;
		mParentNode = 0;
	}

	if(mPolygonList)
	{
		delete [] mPolygonList;
		mPolygonList = 0;
	}

	if(mTextureGroups)
	{
		delete [] mTextureGroups;
		mTextureGroups = 0;
	}
	
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Render
// Purpose: Draws the tree.
// Inputs: Frustum - The viewing frustum used for culling.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool QuadTreeObject::Render(FrustumObject* Frustum)
{
	unsigned int i;


	// Copy the frustum pointer for use by this object.
	FrustumPtr = Frustum;

	// Reset the number of polygons that have to be drawn to zero.
	for(i=0; i<mNumTexGroups; i++)
	{
		mTextureGroups[i].numPolygonsToDraw = 0;
		mTextureGroups[i].VertexBuffer.Reset();
	}

	// Get the current time of the render.
	currentTime = glutGet(GLUT_ELAPSED_TIME);

	// Add only the visible polygons to the buffers for drawing.
	AddNode(mParentNode);

	// Go through each texture group and render the polygons for that texture.
	for(i=0; i<mNumTexGroups; i++)
	{
		// Only set the texture and render its polygons if there are some polgyons 
		// that are visible for that texture.
		if(mTextureGroups[i].numPolygonsToDraw > 0)
		{
			// Bind the texture.
			Terrain->SetTexture(i);

			// Draw the polygons.
			mTextureGroups[i].VertexBuffer.Render(mTextureGroups[i].numPolygonsToDraw);
		}
	}

	// Count all the polys that were drawn.
	mTotalPolysDrawn = 0;
	for(i=0; i<mNumTexGroups; i++)
	{
		mTotalPolysDrawn += mTextureGroups[i].numPolygonsToDraw;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SortNode
// Purpose: Recursive function used for sorting the tree.
// Inputs: node - The current node it is sorting.
//         xPos - The x position of that node.
//         yPos - The y position of that node.
//         zPos - The z position of that node.
//         size - The radius of that node.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void QuadTreeObject::SortNode(NodeType* node, float xPos, float yPos, 
							  float zPos, float size)
{
	unsigned int numPolys, count, i, j;
	float xOff, yOff, zOff;


	// Store the node position and size.
	node->xPos = xPos;
	node->yPos = yPos;
	node->zPos = zPos;
	node->size = size;

	// See if there are any polygons in this node.
	numPolys = CountPolygons(xPos, yPos, zPos, size);

	// If there are no polygons in this node then return.
	if(numPolys == 0)
	{
		return;
	}

	// Otherwise split the node into smaller nodes if there are too many 
	// polygons or the physical node size is too large.
	if((size > mMaxNodeSize) || (numPolys > mMaxNodePolygons))
	{
		for(i=0; i<4; i++)
		{
			// Calculate the position offsets for the new child node.
			xOff = (((i % 2) < 1) ? -1.0f : 1.0f) * (size / 4.0f);
			zOff = (((i % 4) < 2) ? -1.0f : 1.0f) * (size / 4.0f);
			yOff = (((i % 8) < 4) ? -1.0f : 1.0f) * (size / 4.0f);

			// See if there are any polygons in the new node.
			count = CountPolygons((xPos + xOff), (yPos + yOff), (zPos + zOff), (size / 2.0f));
			if(count > 0)
			{
				// If so then create a new child node and sort it as well.
				node->nodes[i] = new NodeType;
				SortNode(node->nodes[i], (xPos + xOff), (yPos + yOff), (zPos + zOff), 
					     (size / 2.0f));
			}
		}
		return;
	}

	// Create the new polygon list for this node.
	node->numPolygons = numPolys;
	node->polygonList = new PolygonType*[node->numPolygons];

	// Find out which polygons are inside this node and add them to it.
	j = 0;
	for(i=0; i<mNumPolygons; i++)
	{
		if(IsPolygonContained(&mPolygonList[i], xPos, yPos, zPos, size) == true)
		{
			node->polygonList[j++] = &mPolygonList[i];
		}
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: CountPolygons
// Purpose: Counts the number of polygons in this node.
// Inputs: xPos - The x position of the node.
//         yPos - The y position of the node.
//         zPos - The z position of the node.
//         size - The radius of the node.
// Outputs: count - The number of polygons in this node.
// Details: NOne.
///////////////////////////////////////////////////////////////////////////////
unsigned int QuadTreeObject::CountPolygons(float xPos, float yPos, float zPos, 
										   float size)
{
	unsigned int count, i;


	// Initialize the count.
	count = 0;
	
	// Go through all the polygons in the mesh and check which ones are inside
	// this node.
	for(i=0; i<mNumPolygons; i++)
	{
		if(IsPolygonContained(&mPolygonList[i], xPos, yPos, zPos, size) == true)
		{
			// If one is found then increment the count.
			count++;
		}
	}

	return count;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: IsPolygonContained
// Purpose: Checks if a polygon is inside a node.
// Inputs: polygon - The polygon being checked.
//         xPos - The x position of the node.
//         yPos - The y position of the node.
//         zPos - The z position of the node.
//         size - The radius of the node.
// Outputs: A boolean value indicating if this polygon is in this node.
// Details: Don't need to check the y-axis because in a quad tree, the height
//          doesn't affect the space partitioning.
///////////////////////////////////////////////////////////////////////////////
bool QuadTreeObject::IsPolygonContained(PolygonType* polygon, float xPos, 
										float yPos, float zPos, float size)
{
	float xMin, xMax, zMin, zMax;
	VertexType vertex[3];
	
	
	// Get the vertices for the polygon.
	Terrain->GetVertexData(polygon->verIndex[0], vertex[0].x, vertex[0].y, vertex[0].z);
	Terrain->GetVertexData(polygon->verIndex[1], vertex[1].x, vertex[1].y, vertex[1].z);
	Terrain->GetVertexData(polygon->verIndex[2], vertex[2].x, vertex[2].y, vertex[2].z);

	// Check to see if the x-axis of the polygon is in the space being checked.
	xMin = min(vertex[0].x, min(vertex[1].x, vertex[2].x));
	xMax = max(vertex[0].x, max(vertex[1].x, vertex[2].x));
	if(xMax < (xPos - size / 2.0f))
	{
		return false;
	}
	if(xMin > (xPos + size / 2.0f))
	{
		return false;
	}

	// Check to see if the x-axis of the polygon is in the space being checked.
	zMin = min(vertex[0].z, min(vertex[1].z, vertex[2].z));
	zMax = max(vertex[0].z, max(vertex[1].z, vertex[2].z));
	if(zMax < (zPos - size / 2.0f))
	{
		return false;
	}
	if(zMin > (zPos + size / 2.0f))
	{
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: AddNode
// Purpose: Adds nodes to be drawn if they can be seen by the viewer.
// Inputs: node - The current node being checked.
// Outputs: None.
// Details: This is a recursive function.
///////////////////////////////////////////////////////////////////////////////
void QuadTreeObject::AddNode(NodeType* node)
{
	bool result;
	unsigned int num, i, group;
	unsigned int vIndex1, vIndex2, vIndex3;
	unsigned int tIndex1, tIndex2, tIndex3;
	unsigned int nIndex1, nIndex2, nIndex3;
	float x, y, z, tu, tv, nx, ny, nz;


	// Check to see if the node can be viewed by the player.  Don't need to 
	// check yPos since height doesn't matter in a quad tree.
	result = FrustumPtr->CheckRectangle(node->xPos, 0.0f, node->zPos, 
									    (node->size / 2.0f), 
										(mMeshCubeSize / 2.0f), 
										(node->size / 2.0f));
	if(!result)
	{
		return;
	}

	// Scan the child nodes to see if they can be viewed.
	num = 0;
	for(i=0; i<4; i++)
	{
		if(node->nodes[i] != 0)
		{
			num++;
			AddNode(node->nodes[i]);
		}
	}

	// If there were no children then there is no need to continue.
	if(num)
	{
		return;
	}

	// If there were polygons in view then add their data to the list to be
	// drawn.
	if(node->numPolygons != 0)
	{
		for(i=0; i<node->numPolygons; i++)
		{
			group = node->polygonList[i]->textureGroup;

			// If the polygon has not been drawn yet then add it to the vertex
			// buffer to be drawn.
			if(node->polygonList[i]->timeLastDrawn != currentTime)
			{
				// Get the indexes of the polygons.
				vIndex1 = node->polygonList[i]->verIndex[0];
				vIndex2 = node->polygonList[i]->verIndex[1];
				vIndex3 = node->polygonList[i]->verIndex[2];
				tIndex1 = node->polygonList[i]->texIndex[0];
				tIndex2 = node->polygonList[i]->texIndex[1];
				tIndex3 = node->polygonList[i]->texIndex[2];
				nIndex1 = node->polygonList[i]->norIndex[0];
				nIndex2 = node->polygonList[i]->norIndex[1];
				nIndex3 = node->polygonList[i]->norIndex[2];

				
				Terrain->GetVertexData(vIndex1, x, y, z);
				Terrain->GetTextureCoordinateData(tIndex1, tu, tv);
				Terrain->GetNormalData(nIndex1, nx, ny, nz);
				mTextureGroups[group].VertexBuffer.AddData(x, y, z, tu, tv, nx, ny, nz);
				
				Terrain->GetVertexData(vIndex2, x, y, z);
				Terrain->GetTextureCoordinateData(tIndex2, tu, tv);
				Terrain->GetNormalData(nIndex2, nx, ny, nz);
				mTextureGroups[group].VertexBuffer.AddData(x, y, z, tu, tv, nx, ny, nz);

				Terrain->GetVertexData(vIndex3, x, y, z);
				Terrain->GetTextureCoordinateData(tIndex3, tu, tv);
				Terrain->GetNormalData(nIndex3, nx, ny, nz);
				mTextureGroups[group].VertexBuffer.AddData(x, y, z, tu, tv, nx, ny, nz);

				mTextureGroups[group].numPolygonsToDraw++;

				node->polygonList[i]->timeLastDrawn = currentTime;
			}
		}
	}
	
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetPolysDrawn
// Purpose: Gives the caller the number of polygons currently being drawn.
// Inputs: None.
// Outputs: mTotalPolysDrawn - The total number of polygons being drawn.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
unsigned int QuadTreeObject::GetPolysDrawn()
{
	return mTotalPolysDrawn;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetHeightAtPosition
// Purpose: Gives the caller the height of the triangle at the given position.
// Inputs: x - The x position of the player.
//         z - The z position of the player.
// Outputs: height - The height at that position.
// Details: This is used to determine the height at which the player is 
//          currently located so it appears as though the player is walking
//          over the land moving up and down as the land does.
///////////////////////////////////////////////////////////////////////////////
bool QuadTreeObject::GetHeightAtPosition(float x, float z, float& height)
{
	// Make sure the coordinates are actually over a polygon.
	if((x < meshMinX) || (x > meshMaxX) || (z < meshMinZ) || (z > meshMaxZ))
	{
		return false;
	}

	// Find the node which contains the polygon the player is standing above.
	FindNode(mParentNode, x, z, height);
	
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: FindNode
// Purpose: Finds the node at the given position so the polygons inside it can
//          be tested for their height.
// Inputs: node - The current node being checked.
//         x - The x position of the player.
//         z - The z position of the player.
// Outputs: height - The height at that position.
// Details: This is a recursive function.
///////////////////////////////////////////////////////////////////////////////
void QuadTreeObject::FindNode(NodeType* node, float x, float z, float& height)
{
	float xMin, xMax, zMin, zMax;
	unsigned int i;
	bool hasChildren;
	unsigned int vIndex1, vIndex2, vIndex3;
	float v0[3], v1[3], v2[3];


	// Calculate the dimensions of this node.
	xMin = node->xPos - node->size;
	xMax = node->xPos + node->size;

	zMin = node->zPos - node->size;
	zMax = node->zPos + node->size;
	
	// See if the x and z coordinate are in this node or not.  If not then stop
	// traversing this part of the tree.
	if((x < xMin) || (x > xMax) || (z < zMin) || (z > zMax))
	{
		return;
	}

	// If the coordinates are in this node then check the children nodes if 
	// they exist.
	hasChildren = false;
	for(i=0; i<4; i++)
	{
		if(node->nodes[i] != 0)
		{
			hasChildren = true;
			FindNode(node->nodes[i], x, z, height);
		}
	}

	// If there were children nodes then return since the polygon will be in
	// one of the children.
	if(hasChildren)
	{
		return;
	}

	// If there were no children then the polygon must be in this node.  Check
	// all the polygons in this node to find the height of which one the 
	// player is standing on.
	for(i=0; i<node->numPolygons; i++)
	{
		// Get the indexes of the polygons.
		vIndex1 = node->polygonList[i]->verIndex[0];
		vIndex2 = node->polygonList[i]->verIndex[1];
		vIndex3 = node->polygonList[i]->verIndex[2];

		// Get the actual vertex data.
		Terrain->GetVertexData(vIndex1, v0[0], v0[1], v0[2]);
		Terrain->GetVertexData(vIndex2, v1[0], v1[1], v1[2]);
		Terrain->GetVertexData(vIndex3, v2[0], v2[1], v2[2]);
				
		if(FoundHeightOfTriangle(x, z, height, v0, v1, v2) == true)
		{
			return;
		}
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: FoundHeightOfTriangle
// Purpose: Gives the height at the point where the player is standing on that 
//          triangle.
// Inputs: x - The x position of the player.
//         z - The z position of the player.
//         v0 - Vertex 1 of the triangle.
//         v1 - Vertex 2 of the triangle.
//         v2 - Vertex 3 of the triangle.
// Outputs: height - The height at that position on the triangle.
// Details: Uses ray intersection of a plane calculations.
///////////////////////////////////////////////////////////////////////////////
bool QuadTreeObject::FoundHeightOfTriangle(float x, float z, float& height, 
										   float v0[3], float v1[3], float v2[3])
{
	float startVector[3], directionVector[3], edge1[3], edge2[3], normal[3];
	float Q[3], e1[3], e2[3], e3[3], edgeNormal[3], temp[3];
	float magnitude, D, denominator, numerator, t, determinant;


	// Starting position of the ray that is being cast.
	startVector[0] = x;
	startVector[1] = 0.0f;
	startVector[2] = z;

	// The direction the ray is being cast.
	directionVector[0] =  0.0f;
	directionVector[1] = -1.0f;
	directionVector[2] =  0.0f;

	// Calculate the two edges from the three points given.
	edge1[0] = v1[0] - v0[0];
	edge1[1] = v1[1] - v0[1];
	edge1[2] = v1[2] - v0[2];

	edge2[0] = v2[0] - v0[0];
	edge2[1] = v2[1] - v0[1];
	edge2[2] = v2[2] - v0[2];

	// Calculate the normal of the triangle from the two edges.
	normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
	normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
	normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

	magnitude = (float)sqrt((normal[0] * normal[0]) + 
						    (normal[1] * normal[1]) + 
							(normal[2] * normal[2]));
	normal[0] = normal[0] / magnitude;
	normal[1] = normal[1] / magnitude;
	normal[2] = normal[2] / magnitude;

	// Find the distance from the origin to the plane.
	D = ((-normal[0] * v0[0]) + (-normal[1] * v0[1]) + (-normal[2] * v0[2]));

	// Get the denominator of the equation.
	denominator = ((normal[0] * directionVector[0]) + 
				   (normal[1] * directionVector[1]) + 
				   (normal[2] * directionVector[2]));

	// Make sure the result doesn't get too close to zero to prevent divide by
	// zero.
	if(fabs(denominator) < 0.0001f)
	{
		return false;
	}

	// Get the numerator of the equation.
	numerator = -1.0f * (((normal[0] * startVector[0]) + 
					      (normal[1] * startVector[1]) + 
					      (normal[2] * startVector[2])) + D);

	// Calculate where we intersect the triangle.
	t = numerator / denominator;

	// Find the intersection vector.
	Q[0] = startVector[0] + (directionVector[0] * t);
	Q[1] = startVector[1] + (directionVector[1] * t);
	Q[2] = startVector[2] + (directionVector[2] * t);

	// Find the three edges of the triangle.
	e1[0] = v1[0] - v0[0];
	e1[1] = v1[1] - v0[1];
	e1[2] = v1[2] - v0[2];

	e2[0] = v2[0] - v1[0];
	e2[1] = v2[1] - v1[1];
	e2[2] = v2[2] - v1[2];

	e3[0] = v0[0] - v2[0];
	e3[1] = v0[1] - v2[1];
	e3[2] = v0[2] - v2[2];

	// Calculate the normal for the first edge.
	edgeNormal[0] = (e1[1] * normal[2]) - (e1[2] * normal[1]);
	edgeNormal[1] = (e1[2] * normal[0]) - (e1[0] * normal[2]);
	edgeNormal[2] = (e1[0] * normal[1]) - (e1[1] * normal[0]);

	// Calculate the determinant to see if it is on the inside, outside, or
	// directly on the edge.
	temp[0] = Q[0] - v0[0];
	temp[1] = Q[1] - v0[1];
	temp[2] = Q[2] - v0[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + 
		           (edgeNormal[2] * temp[2]));

	// Check if it is outside.
	if(determinant > 0.001f)
	{
		return false;
	}

	// Calculate the normal for the second edge.
	edgeNormal[0] = (e2[1] * normal[2]) - (e2[2] * normal[1]);
	edgeNormal[1] = (e2[2] * normal[0]) - (e2[0] * normal[2]);
	edgeNormal[2] = (e2[0] * normal[1]) - (e2[1] * normal[0]);

	// Calculate the determinant to see if it is on the inside, outside, or
	// directly on the edge.
	temp[0] = Q[0] - v1[0];
	temp[1] = Q[1] - v1[1];
	temp[2] = Q[2] - v1[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + 
		           (edgeNormal[2] * temp[2]));

	// Check if it is outside.
	if(determinant > 0.001f)
	{
		return false;
	}

	// Calculate the normal for the third edge.
	edgeNormal[0] = (e3[1] * normal[2]) - (e3[2] * normal[1]);
	edgeNormal[1] = (e3[2] * normal[0]) - (e3[0] * normal[2]);
	edgeNormal[2] = (e3[0] * normal[1]) - (e3[1] * normal[0]);

	// Calculate the determinant to see if it is on the inside, outside, or
	// directly on the edge.
	temp[0] = Q[0] - v2[0];
	temp[1] = Q[1] - v2[1];
	temp[2] = Q[2] - v2[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + 
		           (edgeNormal[2] * temp[2]));

	// Check if it is outside.
	if(determinant > 0.001f)
	{
		return false;
	}

	// Now we have our height.
	height = Q[1];

	return true;
}
