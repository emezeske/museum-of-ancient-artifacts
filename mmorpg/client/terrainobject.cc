///////////////////////////////////////////////////////////////////////////////
// Filename: terrainobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "terrainobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: TerrainObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Sets the member variables to null.
///////////////////////////////////////////////////////////////////////////////
TerrainObject::TerrainObject()
{
	GroundTexture = 0;
	Vertices      = 0;
	TexCoords     = 0;
	Normals       = 0;
	Faces         = 0;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: TerrainObject
// Purpose: Copy constructor.
// Inputs: other - The object this function is making a copy of.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
TerrainObject::TerrainObject(const TerrainObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~TerrainObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: Releases the memory associated with this object.
///////////////////////////////////////////////////////////////////////////////
TerrainObject::~TerrainObject()
{
	if(GroundTexture)
	{
		delete GroundTexture;
		GroundTexture = 0;
	}
	if(Vertices)
	{
		delete [] Vertices;
		Vertices = 0;
	}
	if(TexCoords)
	{
		delete [] TexCoords;
		TexCoords = 0;
	}
	if(Normals)
	{
		delete [] Normals;
		Normals = 0;
	}
	if(Faces)
	{
		delete [] Faces;
		Faces = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Initialize
// Purpose: Loads the terrain data in.
// Inputs: filename - The file name of the terrain file.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool TerrainObject::Initialize(char* filename)
{
	bool result;
	ifstream fin;
	unsigned int water, index, i, j;
	float vertex1[3], vertex2[3], vertex3[3];
	float vector1[3], vector2[3], cross[3], sum[3];
	float magnitude;
	int shared;
	NormalType* tempNormals;
	float maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth;
	float w, h, d, maxX, maxY, maxZ;

	
	// Load a temporary single texture for the terrain.
	GroundTexture = new TextureObject;
	result = GroundTexture->Load("./data/sand001.tga");
	if(!result)
	{
		return false;
	}

	// Open the terrain data file and read in the vertice data.
	fin.open(filename);

	// Read in the water height, width of the terrain, and the height of the 
	// terrain.
	fin >> water;
	fin >> width;
	fin >> height;
	
	// Calculate the number of vertices in the terrain mesh.
	numVerts = height * width;

	// Create a vertex list to hold the terrain vertices.
	Vertices = new VertexType[numVerts];

	// Read in the vertices from the file into the vertex list.
	for(j=0; j<height; j++)
	{
		for(i=0; i<width; i++)
		{
			index = j * height + i;

			fin >> Vertices[index].x;
			fin >> Vertices[index].y;
			fin >> Vertices[index].z;
		}
	}

	// Close the file.
	fin.close();

	// Create a texture coordinate list to hold the texture coordinates for the
	// terrain mesh.
	TexCoords = new TexCoordType[4];

	// Store the uv coordinates in the list.
	TexCoords[0].tu = 0.0f; TexCoords[0].tv = 1.0f;  // Top left.
	TexCoords[1].tu = 0.0f; TexCoords[1].tv = 0.0f;  // Bottom left.
	TexCoords[2].tu = 1.0f; TexCoords[2].tv = 0.0f;  // Bottom right.
	TexCoords[3].tu = 1.0f; TexCoords[3].tv = 1.0f;  // Top right.

	// Calculate the number of triangle faces in the terrain mesh.
	numFaces = ((width - 1) * (height - 1)) * 2;

	// Create a face list to hold the indexes of all the triangle faces in the
	// terrain mesh.
	Faces = new FaceType[numFaces];

	// Build the face list of indexes.
	index = 0;
	for(j=0; j<(height - 1); j++)
	{
		for(i=0; i<(width - 1); i++)
		{
			// First triangle in terrain quad.
			Faces[index].vIndex1 = j     * width + i;
			Faces[index].vIndex2 = (j+1) * width + i;
			Faces[index].vIndex3 = (j+1) * width + (i+1);
			Faces[index].tIndex1 = 0;
			Faces[index].tIndex2 = 1;
			Faces[index].tIndex3 = 2;
			Faces[index].nIndex1 = Faces[index].vIndex1;
			Faces[index].nIndex2 = Faces[index].vIndex2;
			Faces[index].nIndex3 = Faces[index].vIndex3;
			index++;

			// Second triangle in terrain quad.
			Faces[index].vIndex1 = (j+1) * width + (i+1);
			Faces[index].vIndex2 = j     * width + (i+1);
			Faces[index].vIndex3 = j     * width + i;
			Faces[index].tIndex1 = 2;
			Faces[index].tIndex2 = 3;
			Faces[index].tIndex3 = 0;
			Faces[index].nIndex1 = Faces[index].vIndex1;
			Faces[index].nIndex2 = Faces[index].vIndex2;
			Faces[index].nIndex3 = Faces[index].vIndex3;
			index++;
		}
	}

	// Store the number of normals from the terrain mesh.
	numNorms = numVerts;

	// Create a normal list to hold the normals for the terrain mesh.
	Normals = new NormalType[numNorms];

	// Now build the normal list using shared lighting.
	tempNormals = new NormalType[numFaces];

	for(i=0; i<numFaces; i++)
	{
		// Get the three points for the face.
		vertex1[0] = Vertices[Faces[i].vIndex1].x;
		vertex1[1] = Vertices[Faces[i].vIndex1].y;
		vertex1[2] = Vertices[Faces[i].vIndex1].z;
		vertex2[0] = Vertices[Faces[i].vIndex2].x;
		vertex2[1] = Vertices[Faces[i].vIndex2].y;
		vertex2[2] = Vertices[Faces[i].vIndex2].z;
		vertex3[0] = Vertices[Faces[i].vIndex3].x;
		vertex3[1] = Vertices[Faces[i].vIndex3].y;
		vertex3[2] = Vertices[Faces[i].vIndex3].z;

		// Calculate the two vectors for the face.
		vector1[0] = vertex1[0] - vertex3[0];
		vector1[1] = vertex1[1] - vertex3[1];
		vector1[2] = vertex1[2] - vertex3[2];
		vector2[0] = vertex3[0] - vertex2[0];
		vector2[1] = vertex3[1] - vertex2[1];
		vector2[2] = vertex3[2] - vertex2[2];

		// Calculate the cross product of those two vectors.
		cross[0] = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
		cross[1] = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
		cross[2] = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

		// Save the un-normalized values.
		tempNormals[i].nx = cross[0];
		tempNormals[i].ny = cross[1];
		tempNormals[i].nz = cross[2];
	}

	// Initialize the values for calculating the shared vertices.
	sum[0] = 0.0f;
	sum[1] = 0.0f;
	sum[2] = 0.0f;
	shared = 0;

	// Go through all the vertices and see which ones share faces.
	for(i=0; i<numVerts; i++)
	{
		for(j=0; j<numFaces; j++)
		{
			if((Faces[j].vIndex1 == i) || (Faces[j].vIndex2 == i) || (Faces[j].vIndex3 == i))
			{
				sum[0] += tempNormals[j].nx;
				sum[1] += tempNormals[j].ny;
				sum[2] += tempNormals[j].nz;
				shared++;
			}
		}

		// Temporarily store the sum of the vertices for that normal.
		Normals[i].nx = sum[0] / (float)-shared;
		Normals[i].ny = sum[1] / (float)-shared;
		Normals[i].nz = sum[2] / (float)-shared;

		// Normalize it.
		magnitude = (float)sqrt((Normals[i].nx * Normals[i].nx + 
						         Normals[i].ny * Normals[i].ny + 
						         Normals[i].nz * Normals[i].nz));
		Normals[i].nx = Normals[i].nx / magnitude;
		Normals[i].ny = Normals[i].ny / magnitude;
		Normals[i].nz = Normals[i].nz / magnitude;

		// Reset the sums for the next normal.
		sum[0] = 0.0f;
		sum[1] = 0.0f;
		sum[2] = 0.0f;
		shared = 0;
	}

	// Delete the temporary normals memory.
	if(tempNormals)
	{
		delete [] tempNormals;
		tempNormals = 0;
	}

	// Now calculate the dimensions of the terrain mesh, first find the center
	// point of the mesh.
        center[0] = 0.0f;
	center[1] = 0.0f;
	center[2] = 0.0f;

	// Sum all the vertices in the mesh.
	for(i=0; i<numVerts; i++)
	{
		center[0] += Vertices[i].x;
		center[1] += Vertices[i].y;
		center[2] += Vertices[i].z;
	}

	// And then divide it by the number of vertices to find the average 
	// mid-point of the terrain mesh.
	center[0] /= (float)numVerts;
	center[1] /= (float)numVerts;
	center[2] /= (float)numVerts;

	// Now find the bounding cube volume around the terrain mesh.
	maxWidth  = 0.0f;
	maxHeight = 0.0f;
	maxDepth  = 0.0f;

	minWidth  = fabsf(Vertices[0].x - center[0]);
	minHeight = fabsf(Vertices[0].y - center[1]);
	minDepth  = fabsf(Vertices[0].z - center[2]);

	// Go through all the vertices and find the maximum and minimum of x, y, 
	// and z.
	for(i=0; i<numVerts; i++)
	{
		w = fabsf(Vertices[i].x - center[0]);	
		h = fabsf(Vertices[i].y - center[1]);		
		d = fabsf(Vertices[i].z - center[2]);	

		if(w > maxWidth)  { maxWidth  = w; }
		if(h > maxHeight) { maxHeight = h; }
		if(d > maxDepth)  { maxDepth  = d; }
		if(w < minWidth)  { minWidth  = w; }
		if(h < minHeight) { minHeight = h; }
		if(d < minDepth)  { minDepth  = d; }
	}

	// Find the maximum x, y, and z.
	maxX = (float)max(fabs(minWidth),  fabs(maxWidth));
	maxY = (float)max(fabs(minHeight), fabs(maxHeight));
	maxZ = (float)max(fabs(minDepth),  fabs(maxDepth));
	
	// Calculate the maximum diameter of the terrain mesh.
	terrainDiameter = max(maxX, max(maxY, maxZ)) * 2.0f;

	// Get the dimensions of the perimeter.
	minimumX = maximumX = Vertices[0].x;
	minimumZ = maximumZ = Vertices[0].z;
	
	for(i=0; i<numVerts; i++)
	{
		if(Vertices[i].x < minimumX) { minimumX = Vertices[i].x; }
		if(Vertices[i].x > maximumX) { maximumX = Vertices[i].x; }
		if(Vertices[i].z < minimumZ) { minimumZ = Vertices[i].x; }
		if(Vertices[i].z > maximumZ) { maximumZ = Vertices[i].x; }
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Shutdown
// Purpose: Releases the memory associated with the terrain.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool TerrainObject::Shutdown()
{
	if(GroundTexture)
	{
		GroundTexture->Free();
		delete GroundTexture;
		GroundTexture = 0;
	}

	if(Vertices)
	{
		delete [] Vertices;
		Vertices = 0;
	}
	
	if(TexCoords)
	{
		delete [] TexCoords;
		TexCoords = 0;
	}

	if(Normals)
	{
		delete [] Normals;
		Normals = 0;
	}

	if(Faces)
	{
		delete [] Faces;
		Faces = 0;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetNumFaces
// Purpose: Gives the caller the number of faces in the terrain mesh.
// Inputs: None.
// Outputs: numFaces - The number of faces in the mesh.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
unsigned int TerrainObject::GetNumFaces()
{
	return numFaces;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetVertexIndex
// Purpose: Retrieves the vertex indicies of the face requested.
// Inputs: pos - The triangle being requested.
// Outputs: index1 - Index 1 of the triangle.
//          index2 - Index 2 of the triangle.
//          index3 - Index 3 of the triangle.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void TerrainObject::GetVertexIndex(unsigned int pos, unsigned int& index1, 
								   unsigned int& index2, unsigned int& index3)
{
	index1 = Faces[pos].vIndex1;
	index2 = Faces[pos].vIndex2;
	index3 = Faces[pos].vIndex3;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetTextureIndex
// Purpose: Retrieves the texture indicies of the face requested.
// Inputs: pos - The triangle being requested.
// Outputs: index1 - Index 1 of the triangle.
//          index2 - Index 2 of the triangle.
//          index3 - Index 3 of the triangle.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void TerrainObject::GetTextureIndex(unsigned int pos, unsigned int& index1, 
								    unsigned int& index2, unsigned int& index3)
{
	index1 = Faces[pos].tIndex1;
	index2 = Faces[pos].tIndex2;
	index3 = Faces[pos].tIndex3;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetNormalIndex
// Purpose: Retrieves the normal indicies of the face requested.
// Inputs: pos - The triangle being requested.
// Outputs: index1 - Index 1 of the triangle.
//          index2 - Index 2 of the triangle.
//          index3 - Index 3 of the triangle.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void TerrainObject::GetNormalIndex(unsigned int pos, unsigned int& index1, 
								   unsigned int& index2, unsigned int& index3)
{
	index1 = Faces[pos].nIndex1;
	index2 = Faces[pos].nIndex2;
	index3 = Faces[pos].nIndex3;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetTextureIndex
// Purpose: Retrieves the index of the texture.
// Inputs: index - The texture index being looked for.
// Outputs: The index number of the texture.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
unsigned int TerrainObject::GetTextureIndex(unsigned int index)
{
	// Since there is only one texture for now return its index as zero.
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetTextureCount
// Purpose: Returns the number of textures used in the terrain mesh.
// Inputs: None.
// Outputs: Number of textures in terrain.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
unsigned int TerrainObject::GetTextureCount()
{
	// Since there is only one texture for now return one.
	return 1;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetDimensions
// Purpose: Returns the dimensions of the terrain mesh.
// Inputs: None.
// Outputs: centerX - Center of terrain on x-axis.
//          centerY - Center of terrain on y-axis.
//          centerZ - Center of terrain on z-axis.
//          size - The radius of the terrain.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void TerrainObject::GetDimensions(float& centerX, float& centerY, 
								  float& centerZ, float& size)
{
	centerX = center[0];
	centerY = center[1];
	centerZ = center[2];
	size = terrainDiameter;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetVertexData
// Purpose: Retrieves the vertex data for the index given.
// Inputs: index - The index of the vertex the caller wants.
// Outputs: x - The x coordinate of the vertex.
//          y - The y coordinate of the vertex.
//          z - The z coordinate of the vertex.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void TerrainObject::GetVertexData(unsigned int index, float& x, float& y, float& z)
{
	x = Vertices[index].x;
	y = Vertices[index].y;
	z = Vertices[index].z;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetTextureCoordinateData
// Purpose: Retrieves the texture coordinate data for the index given.
// Inputs: index - The index of the texture coordinate the caller wants.
// Outputs: tu - The u coordinate of the texture.
//          tv - The v coordinate of the texture.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void TerrainObject::GetTextureCoordinateData(unsigned int index, float& tu, float& tv)
{
	tu = TexCoords[index].tu;
	tv = TexCoords[index].tv;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetNormalData
// Purpose: Retrieves the normal data for the index given.
// Inputs: index - The index of the normal the caller wants.
// Outputs: nx - The x coordinate of the normal.
//          ny - The y coordinate of the normal.
//          nz - The z coordinate of the normal.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void TerrainObject::GetNormalData(unsigned int index, float& nx, float& ny, float& nz)
{
	nx = Normals[index].nx;
	ny = Normals[index].ny;
	nz = Normals[index].nz;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetTexture
// Purpose: Binds the texture from the given index.
// Inputs: textureNumber - The index of the texture to bind.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void TerrainObject::SetTexture(unsigned int textureNumber)
{
	// Since we only have one texture right now we bind that one.
	GroundTexture->Bind();
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetPerimeter
// Purpose: Gets the perimeter of the terrain mesh.
// Inputs: None.
// Outputs: inMinX - The minimum x size.
//          inMaxX - The maximum x size.
//          inMinZ - The minimum z size.
//          inMaxZ - The maximum z size.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void TerrainObject::GetPerimeter(float& inMinX, float& inMaxX, float& inMinZ, 
								 float& inMaxZ)
{
	inMinX = minimumX;
	inMaxX = maximumX;
	inMinZ = minimumZ;
	inMaxZ = maximumZ;
	return;
}
