///////////////////////////////////////////////////////////////////////////////
// Filename: staticmodelobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "staticmodelobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: StaticModelObject
// Purpose: Class constructor.
// Inputs: None.
// Outputs: None.
// Details: Initializes the member variables.
///////////////////////////////////////////////////////////////////////////////
StaticModelObject::StaticModelObject()
{
	TextureGroups = NULL;
	groupCount = 0;
	xPos = 0.0f;
	yPos = 0.0f;
	zPos = 0.0f;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: StaticModelObject
// Purpose: Copy constructor.
// Inputs: other - The object this function is making a copy of.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
StaticModelObject::StaticModelObject(const StaticModelObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~StaticModelObject
// Purpose: Class destructor.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
StaticModelObject::~StaticModelObject()
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Initialize
// Purpose: Loads the model in.
// Inputs: filename - The filename of the model.
// Outputs: A boolean value indicating if the model was loaded.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool StaticModelObject::Initialize(char* filename)
{
	char directory[80];
	ifstream fin;
	char input;
	int vCount, tCount, nCount, fCount, mCount;
	int i, j;
	char texFilename[80];
	bool done;
	int vPos, tPos, nPos, fPos;
	int currentGroup;
	bool result;
	char file[80];
	int vIndex1, vIndex2, vIndex3, tIndex1, tIndex2, tIndex3, nIndex1, nIndex2, nIndex3;
	int index;

	// First parse out the name of the directory to find the textures in.
	strcpy(directory, filename);

	int currentDir = 0;
	for(i = 0; directory[i] != '\0'; ++i)
	{
		if(directory[i] == '/')
			currentDir = i;
	}
	directory[currentDir+1] = '\0';

	// Open the file for reading.
	fin.open(filename);

	// Read in the vertice count.
	fin >> vCount;
	input = '\0';
	while(input != '\n')
	{
		fin.get(input);
	}

	// Read in the texture coordinate count.
	fin >> tCount;
	input = '\0';
	while(input != '\n')
	{
		fin.get(input);
	}

	// Read in the normal count.
	fin >> nCount;
	input = '\0';
	while(input != '\n')
	{
		fin.get(input);
	}

	// Read in the face count.
	fin >> fCount;
	input = '\0';
	while(input != '\n')
	{
		fin.get(input);
	}

	// Read in the number of textures for the model.
	fin >> groupCount;
	input = '\0';
	while(input != '\n')
	{
		fin.get(input);
	}

	fin.get(input);

	// Store the number of polygons in the model.
	numPolys = fCount;

	VertType* Vertices  = new VertType[vCount];
	TexType*  TexCoords = new TexType[tCount];
	NorType*  Normals   = new NorType[nCount];
	FaceType* Faces     = new FaceType[fCount];
	TextureGroups       = new TextureGroupType[groupCount];

	// Read in and load the textures for each texture group as well as the
	// faces assigned to that group.
	for(i=0; i<groupCount; i++)
	{
		j=0;
		fin.get(input);
		while(input != ' ')
		{
			if(input != '\n') {
				texFilename[j] = input;
				j++;
			}
			fin.get(input);
		}
		texFilename[j] = '\0';

		while(input != '-')
		{
			fin.get(input);
		}

		fin >> mCount;

		while(input != '\n')
		{
			fin.get(input);
		}

		TextureGroups[i].Texture = new TextureObject;

		strcpy(file, directory);
		strcat(file, texFilename);

		result = TextureGroups[i].Texture->Load(file);

		if(!result)
		{
			std::cerr << "Error loading texture in staticmodelobject\n";
		}

		TextureGroups[i].numFaces = mCount;
		TextureGroups[i].numElements = mCount * 3;
		TextureGroups[i].dataBuffer = new float[mCount * 24];
	}
	fin.get(input);


	// Now read in the vertices, normals, tex coords, and faces into data
	// buffers.
	vPos = tPos = nPos = fPos = 0;
	currentGroup = -1;

	done = false;
	while(!done)
	{
		fin.get(input);

		if(fin.eof())
		{
			done = true;
		}
		else
		{
			if(input == 'v')
			{
				fin >> Vertices[vPos].x >> Vertices[vPos].y >> Vertices[vPos].z;
				vPos++;
			}
			if(input == 't')
			{
				fin >> TexCoords[tPos].tu >> TexCoords[tPos].tv;
				tPos++;
			}
			if(input == 'n')
			{
				fin >> Normals[nPos].nx >> Normals[nPos].ny >> Normals[nPos].nz;
				nPos++;
			}
			if(input == 'm')
			{
				currentGroup++;
			}
			if(input == 'f')
			{
				fin >> Faces[fPos].vIndex1 >> input
					>> Faces[fPos].tIndex1 >> input
					>> Faces[fPos].nIndex1
					>> Faces[fPos].vIndex2 >> input
					>> Faces[fPos].tIndex2 >> input
					>> Faces[fPos].nIndex2
					>> Faces[fPos].vIndex3 >> input
					>> Faces[fPos].tIndex3 >> input
					>> Faces[fPos].nIndex3;
				Faces[fPos].textureGroup = currentGroup;
				fPos++;
			}
			while(input != '\n')
			{
				fin.get(input);
			}
		}
	}
	fin.close();

	// Now compute the bounding box for the mesh.
	float center[3];	
	float maxWidth, maxHeight, maxDepth;
	float minWidth, minHeight, minDepth;
	float width, depth, height;
	float maxX, maxY, maxZ;

    center[0] = 0.0f;
	center[1] = 0.0f;
	center[2] = 0.0f;
	for(j=0; j<vCount; j++)
	{
		center[0] += Vertices[j].x;
		center[1] += Vertices[j].y;
		center[2] += Vertices[j].z;
	}
	center[0] /= vCount;
	center[1] /= vCount;
	center[2] /= vCount;

	maxWidth  = 0.0f;
	maxHeight = 0.0f;
	maxDepth  = 0.0f;
	minWidth  = fabsf(Vertices[0].x - center[0]);
	minHeight = fabsf(Vertices[0].y - center[1]);
	minDepth  = fabsf(Vertices[0].z - center[2]);

	for(j=0; j<vCount; j++)
	{
		width  = fabsf(Vertices[j].x - center[0]);	
		height = fabsf(Vertices[j].y - center[1]);		
		depth  = fabsf(Vertices[j].z - center[2]);	

		if(width > maxWidth)
		{
			maxWidth = width;
		}
		if(height > maxHeight)
		{
			maxHeight = height;
		}
		if(depth > maxDepth)
		{
			maxDepth = depth;
		}
		if(width < minWidth)
		{
			minWidth = width;
		}
		if(height < minHeight)
		{
			minHeight = height;
		}
		if(depth < minDepth)
		{
			minDepth = depth;
		}
	}

	centerX = center[0];
	centerY = center[1];
	centerZ = center[2];

	maxX = (float)max(fabs(minWidth),  fabs(maxWidth));
	maxY = (float)max(fabs(minHeight), fabs(maxHeight));
	maxZ = (float)max(fabs(minDepth),  fabs(maxDepth));
	
	cubeSize = max(maxX, max(maxY, maxZ));


	// Now load the texture groups data buffers with the face data.
	for(j=0; j<groupCount; j++)
	{
		index = 0;
		while(Faces[index].textureGroup != j)
		{
			index++;
		}

		for(i=0; i<TextureGroups[j].numFaces; i++)
		{
			vIndex1 = Faces[index].vIndex1;
			vIndex2 = Faces[index].vIndex2;
			vIndex3 = Faces[index].vIndex3;

			tIndex1 = Faces[index].tIndex1;
			tIndex2 = Faces[index].tIndex2;
			tIndex3 = Faces[index].tIndex3;

			nIndex1 = Faces[index].nIndex1;
			nIndex2 = Faces[index].nIndex2;
			nIndex3 = Faces[index].nIndex3;

			TextureGroups[j].dataBuffer[i*24 + 0] = TexCoords[tIndex1].tu;
			TextureGroups[j].dataBuffer[i*24 + 1] = TexCoords[tIndex1].tv;
			TextureGroups[j].dataBuffer[i*24 + 2] = Normals[nIndex1].nx;
			TextureGroups[j].dataBuffer[i*24 + 3] = Normals[nIndex1].ny;
			TextureGroups[j].dataBuffer[i*24 + 4] = Normals[nIndex1].nz;
			TextureGroups[j].dataBuffer[i*24 + 5] = Vertices[vIndex1].x;
			TextureGroups[j].dataBuffer[i*24 + 6] = Vertices[vIndex1].y;
			TextureGroups[j].dataBuffer[i*24 + 7] = Vertices[vIndex1].z;

			TextureGroups[j].dataBuffer[i*24 + 8] = TexCoords[tIndex2].tu;
			TextureGroups[j].dataBuffer[i*24 + 9] = TexCoords[tIndex2].tv;
			TextureGroups[j].dataBuffer[i*24 +10] = Normals[nIndex2].nx;
			TextureGroups[j].dataBuffer[i*24 +11] = Normals[nIndex2].ny;
			TextureGroups[j].dataBuffer[i*24 +12] = Normals[nIndex2].nz;
			TextureGroups[j].dataBuffer[i*24 +13] = Vertices[vIndex2].x;
			TextureGroups[j].dataBuffer[i*24 +14] = Vertices[vIndex2].y;
			TextureGroups[j].dataBuffer[i*24 +15] = Vertices[vIndex2].z;

			TextureGroups[j].dataBuffer[i*24 +16] = TexCoords[tIndex3].tu;
			TextureGroups[j].dataBuffer[i*24 +17] = TexCoords[tIndex3].tv;
			TextureGroups[j].dataBuffer[i*24 +18] = Normals[nIndex3].nx;
			TextureGroups[j].dataBuffer[i*24 +19] = Normals[nIndex3].ny;
			TextureGroups[j].dataBuffer[i*24 +20] = Normals[nIndex3].nz;
			TextureGroups[j].dataBuffer[i*24 +21] = Vertices[vIndex3].x;
			TextureGroups[j].dataBuffer[i*24 +22] = Vertices[vIndex3].y;
			TextureGroups[j].dataBuffer[i*24 +23] = Vertices[vIndex3].z;

			index++;
		}
	}
	
	// Release the memory.
	if(Vertices)
	{
		delete [] Vertices;
		Vertices = NULL;
	}
	if(TexCoords)
	{
		delete [] TexCoords;
		TexCoords = NULL;
	}
	if(Normals)
	{
		delete [] Normals;
		Normals = NULL;
	}
	if(Faces)
	{
		delete [] Faces;
		Faces = NULL;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Shutdown
// Purpose: Releases the memory associated with the object.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool StaticModelObject::Shutdown()
{
	int i;

	// Release all the textures and model data.
	if(TextureGroups)
	{
		for(i=0; i<groupCount; i++)
		{
			if(TextureGroups[i].Texture)
			{
				TextureGroups[i].Texture->Free();
				delete TextureGroups[i].Texture;
				TextureGroups[i].Texture = NULL;
			}
			if(TextureGroups[i].dataBuffer)
			{
				delete [] TextureGroups[i].dataBuffer;
				TextureGroups[i].dataBuffer = NULL;
			}
		}
		groupCount = 0;

		delete TextureGroups;
		TextureGroups = NULL;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Render
// Purpose: Draws the model.
// Inputs: None.
// Outputs: None.
// Details: Draws according to texture groups using interleaving arrays for
//          efficency and speed.
///////////////////////////////////////////////////////////////////////////////
void StaticModelObject::Render()
{
	int i;


	// Save the opengl matrix.
	glPushMatrix();

		// Go to the position of the model.
		glTranslatef(xPos, yPos, zPos);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		// Go through each texture associated with the model and draw the 
		// polygons that use that texture.
		for(i=0; i<groupCount; i++)
		{
			TextureGroups[i].Texture->Bind();
		
			glInterleavedArrays(GL_T2F_N3F_V3F, 0, TextureGroups[i].dataBuffer);
			
			glDrawArrays(GL_TRIANGLES, 0, TextureGroups[i].numElements);
		}

	// Retrieve the original opengl matrix.
	glPopMatrix();

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetPosition
// Purpose: Set the position of the model.
// Inputs: posX - The x position to place the model at.
//         posY - The y position to place the model at.
//         posZ - The z position to place the model at.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void StaticModelObject::SetPosition(float posX, float posY, float posZ)
{
	xPos = posX;
	yPos = posY;
	zPos = posZ;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetPolyCount
// Purpose: Gives the caller the number of polygons in the model.
// Inputs: None.
// Outputs: numPolys - The number of polygons in the model.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
int StaticModelObject::GetPolyCount()
{
	return numPolys;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetBoundingVolume
// Purpose: Gives the caller the bounding volume of the model.
// Inputs: None.
// Outputs: x - The x diameter.
//          y - The y diameter.
//          z - The z diameter.
//          size - The radius of the bounding volume.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void StaticModelObject::GetBoundingVolume(float& x, float& y, float& z, float& size)
{
	x = centerX + xPos;
	y = centerY + yPos;
	z = centerZ + zPos;
	size = cubeSize;
	return;
}
